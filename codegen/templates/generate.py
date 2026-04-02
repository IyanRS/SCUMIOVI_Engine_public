#!/usr/bin/env python3
import os
import csv
import shutil
import subprocess
import sys
import re
from jinja2 import Environment, FileSystemLoader

# --- Paths and settings ---
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
GAME_FILES_FOLDER = os.path.join(SCRIPT_DIR, '..', '..', 'game')
TEMPLATES_FOLDER = SCRIPT_DIR
OUTPUT_DIR = os.path.normpath(os.path.join(SCRIPT_DIR, '..', 'src', 'generated'))
os.makedirs(OUTPUT_DIR, exist_ok=True)

CSV_SEPARATOR = ','
VALUES_SEPARATOR = ';'

WALKABLE_AREA_FILE = 'WalkableArea.csv'
BACKGROUND_FILE = 'Background.csv'
SCENE_OBJECTS_FILE = 'GameObjects.csv'

# --- Parsing helpers ---
def parse_vector(coords, type_hint=""):
    x, y = coords.strip().split(VALUES_SEPARATOR)
    return f'Vector2<{type_hint}>({x},{y})'

# Parse single animation line into a dict
def parse_animation(line):
    values = [v.strip() for v in line.split(CSV_SEPARATOR)]
    name, src = values[0], values[1]
    if 'mirror' in src:
        _, orig = src.split(VALUES_SEPARATOR)
        return {'name': name, 'mirror_of': orig}
    return {
        'name': name,
        'src': src,
        'pos': values[2],
        'size': values[3],
        'frames': values[4]
    }

# Parse single player line into a dict
def parse_player(line):
    return {
        'id': line['id'].strip(),
        'animationMoveRight': line['animationMoveRight'].strip(),
        'animationMoveLeft': line['animationMoveLeft'].strip(),
        'animationIdleRight': line['animationIdleRight'].strip(),
        'animationIdleLeft': line['animationIdleLeft'].strip(),
        'text_format': line['text_format'].strip()
    }

# Parse single object line into a dict
def parse_object(line):
    if line['type'] == '':
        return {
            'id': line['id'].strip(),
            'type': line['type'].strip(),
            'image': line['image'].strip(),
            'offset': line['offset'].strip(),
            'size': line['size'].strip(),
            'text_format': line['text_format'].strip()
        }
    elif line['type'].split(VALUES_SEPARATOR)[0] == 'item':
        return {
            'id': line['id'].strip(),
            'type': line['type'].split(VALUES_SEPARATOR)[0].strip(),
            'itemImage': line['type'].split(VALUES_SEPARATOR)[1].strip(),
            'image': line['image'].strip(),
            'offset': line['offset'].strip(),
            'size': line['size'].strip(),
            'text_format': line['text_format'].strip()
        }

def parse_scene(scene_folder, player_ids):
    """
    Returns a dict:
      {
        'name': 'scummBar',
        'walk': [ '100;113', '116;125', … ],
        'bg':   [ {'tex':'SCUMM_Bar.png','pos':'0;0'}, … ],
        'player': {'id':'Guybrush','pos':'80;80'},
        'objects': [ {'id':'table','tex':'table.png','pos':'157;115'}, … ]
      }
    """
    scene_name = os.path.basename(scene_folder)
    walk = []
    bg   = []
    # 1) walkable area
    wa_path = os.path.join(scene_folder, WALKABLE_AREA_FILE)
    if os.path.exists(wa_path):
        with open(wa_path, newline='') as f:
            for line in f:
                walk.append(line.strip())   # keep the raw "x;y" for parse_vector()

    # 2) backgrounds
    bg_path = os.path.join(scene_folder, BACKGROUND_FILE)
    if os.path.exists(bg_path):
        with open(bg_path, newline='') as f:
            reader = csv.reader(f)
            for fields in reader:
                tex, pos = fields[0].strip(), fields[1].strip()
                bg.append({'tex': tex, 'pos': pos})

    # 3) objects (including the player)
    player_obj = None
    objects    = []
    obj_path = os.path.join(scene_folder, SCENE_OBJECTS_FILE)
    if os.path.exists(obj_path):
        with open(obj_path, newline='') as f:
            reader = csv.DictReader(f)
            # Expect headers: id,texture[,itemTexture],pos
            for row in reader:
                oid = row['id'].strip()
                entry = {
                    'id': oid,
                    'pos': row['pos'].strip(),
                    'layer': row['layer'].strip()
                }
                if oid in player_ids:
                    # this is the player
                    player_obj = { 'id': oid, 'pos': entry['pos'], 'layer': entry['layer'] }
                else:
                    objects.append(entry)

    return {
        'name':   scene_name,
        'walk':   walk,
        'bg':     bg,
        'player': player_obj,
        'objects': objects
    }

def parse_settings():
    config = {}
    path = os.path.join(GAME_FILES_FOLDER, 'Settings.csv')
    if os.path.exists(path):
        with open(path, newline='') as f:
            reader = csv.DictReader(f)
            for row in reader:
                key = row.get('key') or row.get('Key')
                val = row.get('value') or row.get('Value')
                if key and val:
                    config[key] = val
    return config

scripts = {}

def change_indentation(code_str, indent_with='\t\t'):
    """
    Prepends 'indent_with' to each non-blank line of the given code string, preserving existing indentation.
    """
    lines = code_str.splitlines()
    indented = []
    for line in lines:
        if line.strip():
            indented.append(f"{indent_with}{line}")
        else:
            indented.append(line)
    return '\n'.join(indented)

def get_dialogue_script(filename):
    return 	f'vars:mainNode\ne.player.disableMovement();\ne.game.getUI().setLayout(e.game.getUI().getDialogueLayout()->getName());\nmainNode->enterNode();'

def separate_script_vars(script):
    vars = []
    lines = script.splitlines()
    if lines[0].strip().startswith('vars:'):
        vars = lines[0].strip()[5:].strip().split(',')
        # Remove the first line with vars from the script
        script = '\n'.join(lines[1:])
        return script, vars
    return script, []

def parse_interaction(row):
    """
    Parses a single interaction row.
    Expected fields: id, type, target, text, animation, sound
    """
    script = change_indentation(get_dialogue_script(row['script'].strip())) if 'dialogue(' in row['script'] else change_indentation(scripts[row['script'].strip()])
    script, vars = separate_script_vars(script)
    return {
        'object_id': row['object_id'].strip(),
        'verb': row['verb'].strip(),
        'other_object_id': row['other_object_id'].strip(),
        'script_file': row['script'].strip(),
        'vars': vars,
        'script': script
    }

def parse_scripts(scripts_folder):
    """
    Parses all script files in the given folder.
    Returns a list of script names.
    """
    global scripts

    for filename in os.listdir(scripts_folder):
        # Read the content of the script and append it to the scripts dictionary with the filename as key
        script_path = os.path.join(scripts_folder, filename)
        with open(script_path, 'r', encoding='utf-8') as f:
            content = f.read()
            scripts[filename] = content
    return scripts

def parse_text_format(row):
    """
    Parses a single text format row.
    Expected fields: format_id,file,size,color,outline,position,shadow
    Examples:
        default,arial.ttf,12,0xffffff,true,top,true
        stan_text_format,arial.ttf,12,0x3e97e6,true,top,true
    """
    return {
        'format_id': row['format_id'].strip(),
        'file': row['file'].strip(),
        'size': row['size'].strip(),
        # Convert color to hex to 3 element int array (e.g. 0xffffff -> [255, 255, 255])
        'color': [int(row['color'].strip()[i:i+2], 16) for i in (2, 4, 6)] if row['color'].strip().startswith('0x') else [int(row['color'].strip()[i:i+2], 16) for i in (0, 2, 4)],
        'outline': row['outline'].strip().lower() == 'true',
        'position': row['position'].strip(),
        'shadow': row['shadow'].strip().lower() == 'true'
    }

# --- Dialogue parsing helpers ---
class DialogueLine:
    def __init__(self, speaker, text, delay=None, target=None, action=None):
        self.speaker = speaker
        self.text = text
        self.delay = float(delay) if delay else None
        self.target = target
        self.action = action

class DialogueOption:
    def __init__(self, text, target_label):
        self.text = text
        self.target = target_label

class DialogueLabel:
    def __init__(self, name):
        self.name = name
        self.lines = []
        self.options = []
        self.is_exit = False

def parse_dialogue_file(path):
    labels = {}
    current = None
    option_re = re.compile(r'^\s*"([^"]+)":\s*(\w+)$')
    line_re = re.compile(r'^\s*(?:(\w+):)?\s*"([^"]*)"(?:\s*\(([^)]+)\))?(?:\s*:\s*(\w+))?$')
    exec_re = re.compile(r'execute\(([^)]+)\)')
    exit_re = re.compile(r'exit')

    with open(path, 'r', encoding='utf-8') as f:
        for raw in f:
            line = raw.strip()
            if not line:
                continue
            if line.endswith(':') and not line.startswith('"'):
                lbl = line[:-1]
                current = DialogueLabel(lbl)
                labels[lbl] = current
                continue
            if exit_re.fullmatch(line):
                current.is_exit = True
                continue
            m_exec = exec_re.search(line)
            if m_exec:
                current.lines.append(DialogueLine(None, None, action=m_exec.group(1)))
                continue
            m_opt = option_re.match(line)
            if m_opt:
                text, target = m_opt.groups()
                current.options.append(DialogueOption(text, target))
                continue
            m = line_re.match(line)
            if m:
                speaker, text, delay, jump = m.groups()
                if not speaker and jump:
                    current.options.append(DialogueOption(text, jump))
                else:
                    current.lines.append(DialogueLine(speaker or 'player', text, delay, target=jump))
                continue
            print(f"Warning: unparsed: {line}")
    return labels

# Collect all data
def collect_data():
    animations, players, objects, scenes, interactions, text_formats, dialogues = [], [], [], [], [], [], []
    # Animations
    with open(os.path.join(GAME_FILES_FOLDER, 'Animations.csv')) as f:
        for l in f:
            animations.append(parse_animation(l))
    with open(os.path.join(GAME_FILES_FOLDER, 'TextFormats.csv')) as f:
        reader = csv.DictReader(f)
        for row in reader:
            text_formats.append(parse_text_format(row))
    # Players
    with open(os.path.join(GAME_FILES_FOLDER, 'Players.csv')) as f:
        reader = csv.DictReader(f)
        for row in reader:
            players.append(parse_player(row))
    with open(os.path.join(GAME_FILES_FOLDER, 'Objects.csv')) as f:
        reader = csv.DictReader(f)
        for row in reader:
            objects.append(parse_object(row))
    parse_scripts(os.path.join(GAME_FILES_FOLDER, 'Scripts'))
    with open(os.path.join(GAME_FILES_FOLDER, 'Interactions.csv')) as f:
        reader = csv.DictReader(f)
        for row in reader:
            interactions.append(parse_interaction(row))
    player_ids = { p['id'] for p in players }
    # Scenes
    scenes_dir = os.path.join(GAME_FILES_FOLDER, 'Scenes')
    for dirname in os.listdir(scenes_dir):
        path = os.path.join(scenes_dir, dirname)
        if os.path.isdir(path):
            scenes.append(parse_scene(path, player_ids))
    # Dialogues
    dialogues_dir = os.path.join(GAME_FILES_FOLDER, 'Dialogues')
    for dirname in os.listdir(dialogues_dir):
        path = os.path.join(dialogues_dir, dirname)
        dialogues.append(parse_dialogue_file(path))
    # Settings
    settings = parse_settings()
    return {'animations': animations, 'players': players, 'scenes': scenes, 'config': settings, 'objects': objects, 'interactions': interactions, 'text_formats': text_formats, 'dialogues': dialogues}

# --- Main: generate via Jinja2 and compile ---
def main():
    data = collect_data()
    # Setup Jinja2 environment
    script_dir = os.path.dirname(os.path.abspath(__file__))
    env = Environment(
        loader=FileSystemLoader(script_dir),
        trim_blocks=True,
        lstrip_blocks=True
    )
    # Make parse_vector available inside templates
    env.globals['parse_vector'] = parse_vector

    template = env.get_template('GameData.cpp.jinja2')
    rendered = template.render(data)

    # Write generated C++
    out_file = os.path.join(OUTPUT_DIR, 'GameData.cpp')
    with open(out_file, 'w') as f:
        f.write(rendered)
    print(f'Generated -> {out_file}')

    # # Optionally compile via MSBuild
    # sln_path = os.path.normpath(os.path.join(os.path.dirname(__file__), '..', 'MyGame.sln'))
    # if os.path.exists(sln_path):
    #     msbuild_solution(sln_path)
    # else:
    #     print(f'! Solution not found at {sln_path}, skipping build.')

# MSVC detection and build

def find_vs_installation():
    vswhere = shutil.which('vswhere')
    if not vswhere:
        raise RuntimeError("vswhere.exe not found on PATH")
    out = subprocess.check_output([
        vswhere,
        '-latest',
        '-products', '*',
        '-requires', 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64',
        '-property', 'installationPath'
    ], encoding='utf-8')
    path = out.strip()
    if not path:
        raise RuntimeError("vswhere did not return a path")
    return path


def msbuild_solution(sln_path, configuration='Debug'):
    vs_path = find_vs_installation()
    devcmd = os.path.join(vs_path, 'Common7', 'Tools', 'VsDevCmd.bat')
    if not os.path.exists(devcmd):
        raise RuntimeError(f"VsDevCmd.bat not found at {devcmd}")
    cmd = (
        f'cmd.exe /C ""{devcmd}" && '
        f'msbuild "{sln_path}" /p:Configuration={configuration} /m"'
    )
    print(f'↪ Building via MSBuild: {cmd}')
    subprocess.run(cmd, shell=True, check=True)

if __name__ == '__main__':
    main()