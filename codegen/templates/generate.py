#!/usr/bin/env python3
import os
import csv
import shutil
import subprocess
import sys
from jinja2 import Environment, FileSystemLoader

# --- Paths and settings ---
GAME_FILES_FOLDER = '../../game'
TEMPLATES_FOLDER = os.path.join(os.path.dirname(__file__), 'templates')
OUTPUT_DIR = os.path.join(os.path.dirname(__file__), '..', 'src', 'generated')
os.makedirs(OUTPUT_DIR, exist_ok=True)

CSV_SEPARATOR = ','
VALUES_SEPARATOR = ';'

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
    values = [v.strip() for v in line.split(CSV_SEPARATOR)]
    return {
        'id': values[0],
        'anims': values[1:5]
    }

# Parse Scenes folder into a structured dict
def parse_scene(scene_path):
    name = os.path.basename(scene_path)
    walk_points = []
    backgrounds = []
    for fname in os.listdir(scene_path):
        full = os.path.join(scene_path, fname)
        if 'WalkableArea.csv' in fname:
            with open(full) as f:
                for l in f:
                    if l.strip(): walk_points.append(l.strip())
        if 'Background.csv' in fname:
            with open(full) as f:
                for l in f:
                    tex, pos = [v.strip() for v in l.split(CSV_SEPARATOR)]
                    backgrounds.append({'tex': tex, 'pos': pos})
    return {'name': name, 'walk': walk_points, 'bg': backgrounds}

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

# Collect all data
def collect_data():
    animations, players, scenes = [], [], []
    # Animations
    with open(os.path.join(GAME_FILES_FOLDER, 'Animations.csv')) as f:
        for l in f:
            animations.append(parse_animation(l))
    # Players
    with open(os.path.join(GAME_FILES_FOLDER, 'Players.csv')) as f:
        for l in f:
            players.append(parse_player(l))
    # Scenes
    scenes_dir = os.path.join(GAME_FILES_FOLDER, 'Scenes')
    for sd in os.listdir(scenes_dir):
        scenes.append(parse_scene(os.path.join(scenes_dir, sd)))
    # Settings
    settings = parse_settings()
    return {'animations': animations, 'players': players, 'scenes': scenes, 'config': settings}

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
    print(f'✓ Generated -> {out_file}')

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