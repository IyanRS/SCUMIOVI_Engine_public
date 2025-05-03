import os
GAME_FILES_FOLDER = '../game'

CSV_SEPARATOR = ','
VALUES_SEPARATOR = ";"

SCENES_FOLDER = "Scenes"

OBJECTS_FILE = 'Objects.csv'
PLAYERS_FILE = 'Players.csv'
ANIMATIONS_FILE = 'Animations.csv'
SCENES_FILE = 'Scenes.csv'
UI_FILE = 'UI.csv'

WALKABLE_AREA_FILE = 'WalkableArea.csv'
BACKGROUND_FILE = 'Background.csv'

def parse_vector(coords, type=""):
    coords = coords.strip()
    coords = coords.split(VALUES_SEPARATOR)
    return f'Vector2<{type}>({coords[0]},{coords[1]})'

# Examples:
# guybrushWalkRight,Guybrush_walk_side.png,0;0,32;47,6
# guybrushWalkLeft,mirror;guybrushWalkRight

def parse_animation(animation_line):
    animation_line = animation_line.strip()

    output = 'Animation '
    values = animation_line.split(CSV_SEPARATOR)

    animation_name = values[0]
    output += animation_name

    animation_source = values[1]
    if 'mirror' in animation_source:
        output += ' = ' + animation_source.split(VALUES_SEPARATOR)[1] + '.getMirrored("' + animation_name + '");\n'
        return output
    
    output += ' = Animation("' + animation_name + '", "' + animation_source + '", '

    animation_source_position = parse_vector(values[2], type="int")
    output += animation_source_position + ', '

    animation_source_size = parse_vector(values[3], type="int")
    output += animation_source_size + ', '

    animation_frames = values[4]
    output += animation_frames + ");\n"

    return output

# Examples:
# guybrush,guybrushWalkRight,guybrushWalkLeft,guybrushIdleRight;guybrushIdleLeft

def parse_player(player_line):
    player_line = player_line.strip()

    values = player_line.split(CSV_SEPARATOR)
    output = f'Player {values[0]} = Player("{values[0]}", Vector2<>(0,0));\n' # TODO: custom player position
    output += f'{values[0]}.setLayer(0);\n'
    for i in range(4):
        output += f'{values[0]}.addAnimation(&{values[1+i]});\n'
    output += f'game.addPlayer({values[0]});\n'
    return output

def parse_scene(scene_folder):
    directory = os.fsdecode(scene_folder)

    scene_name = scene_folder.split("\\")[-1]

    output = f'Scene {scene_name};\n'

    for filename in os.listdir(directory):
        if WALKABLE_AREA_FILE in filename:
            with open(os.path.join(scene_folder, filename), 'r') as f:
                output += 'WalkableArea ' + scene_name + 'WalkableArea = WalkableArea(\n'
                output += '\t{\n'
                for l in f:
                    output += '\t' + parse_vector(l) + ',\n'
                output += '\t}\n);\n'
                output += f'{scene_name}.setWalkableArea(&{scene_name}WalkableArea);\n'
        elif BACKGROUND_FILE in filename:
            with open(os.path.join(scene_folder, filename), 'r') as f:
                for l in f:
                    values = l.split(CSV_SEPARATOR)
                    output += f'GameObject& {scene_name}Background = {scene_name}.addBackground(scene, "{values[0]}", {parse_vector(values[1], type="int")});\n'
    return output


def parse_game():
    directory = os.fsdecode(GAME_FILES_FOLDER)  # Decode the directory to a string

    output = ''

    for filename in os.listdir(directory):
        filename = os.fsdecode(filename)  # Decode each filename to a string

        if ANIMATIONS_FILE in filename:
            with open(os.path.join(directory, filename), 'r') as f:
                for l in f:
                    output += parse_animation(l)
        
        elif PLAYERS_FILE in filename:
            with open(os.path.join(directory, filename), 'r') as f:
                for l in f:
                    output += parse_player(l)
        
    scene_directory = os.fsdecode(f'{GAME_FILES_FOLDER}\\{SCENES_FOLDER}')

    for scene in os.listdir(scene_directory):
        output += parse_scene(os.path.join(scene_directory, scene))
    
    output += "game.startGame();\n"
    return output


if __name__ == '__main__':
    print(parse_game())