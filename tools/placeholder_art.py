#!/usr/bin/env python3
"""
Draws the sample game's placeholder ("programmer") art into assets/textures/.

Every texture the demo loads is generated here from simple shapes, so the
repository ships no third-party art. Sizes match what game/*.csv expects
(object positions, animation frame sizes, background crop). Re-run after
tweaking anything below:

    python tools/placeholder_art.py

Requires Pillow (pip install pillow).
"""
import math
import os
import random

from PIL import Image, ImageDraw

OUT_DIR = os.path.normpath(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'assets', 'textures'))

# --- Palette ---
CLEAR = (0, 0, 0, 0)
OUTLINE = (28, 18, 14)

WOOD_DARK = (66, 40, 24)
WOOD_MID = (104, 64, 36)
WOOD_LIGHT = (140, 92, 52)
WOOD_HIGHLIGHT = (170, 118, 70)

PLASTER = (138, 104, 74)
PLASTER_DARK = (118, 88, 62)
PLASTER_LIGHT = (152, 118, 86)

FLOOR_A = (92, 58, 34)
FLOOR_B = (100, 64, 38)
FLOOR_SEAM = (58, 36, 20)

STONE = (96, 92, 88)
STONE_LIGHT = (122, 116, 108)
STONE_DARK = (64, 60, 58)

NIGHT = (26, 32, 72)
NIGHT_LIGHT = (44, 54, 108)
STAR = (220, 220, 180)

LAMP = (255, 214, 112)
LAMP_HOT = (255, 244, 200)
FIRE_A = (240, 120, 40)
FIRE_B = (255, 190, 70)
DOORWAY = (20, 14, 12)

SKIN = (232, 176, 128)
SKIN_SHADE = (196, 138, 98)


def new(w, h, fill=CLEAR):
    return Image.new('RGBA', (w, h), fill)


def rect(d, x0, y0, x1, y1, color):
    """Inclusive pixel rectangle."""
    d.rectangle([x0, y0, x1, y1], fill=color)


def save(img, name):
    img.save(os.path.join(OUT_DIR, name))
    print(f'  {name:16} {img.width}x{img.height}')


# --- Shared scenery pieces ---

def speckle(img, box, colors, density, seed):
    """Sparse random pixels so flat walls read as a hand-pixelled surface."""
    rnd = random.Random(seed)
    x0, y0, x1, y1 = box
    px = img.load()
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            if rnd.random() < density:
                px[x, y] = rnd.choice(colors) + (255,)


def planks(d, y0, y1, width, seed):
    """Horizontal floor boards with staggered end seams."""
    rnd = random.Random(seed)
    y, row = y0, 0
    while y <= y1:
        h = 5 + row // 2  # boards get taller towards the viewer
        rect(d, 0, y, width - 1, min(y + h - 1, y1), FLOOR_A if row % 2 else FLOOR_B)
        d.line([0, y, width - 1, y], fill=FLOOR_SEAM)
        x = rnd.randint(0, 40)
        while x < width:
            d.line([x, y, x, min(y + h - 1, y1)], fill=FLOOR_SEAM)
            x += rnd.randint(50, 90)
        y += h
        row += 1


def closed_door(d, x0, y0, w, h):
    """Wooden door with a frame drawn just outside the object rect, so the
    open-door texture (drawn exactly over the rect) covers the leaf only."""
    x1, y1 = x0 + w - 1, y0 + h - 1
    rect(d, x0 - 3, y0 - 3, x1 + 3, y1, WOOD_DARK)
    rect(d, x0 - 2, y0 - 2, x1 + 2, y1, OUTLINE)
    rect(d, x0, y0, x1, y1, WOOD_MID)
    for x in range(x0 + 5, x1, 6):
        d.line([x, y0, x, y1], fill=WOOD_DARK)
    for y in (y0 + 6, y1 - 8):
        rect(d, x0, y, x1, y + 1, WOOD_DARK)
    knob_x = x1 - 4
    rect(d, knob_x, y0 + h // 2, knob_x + 1, y0 + h // 2 + 1, LAMP)


def window(d, x0, y0, w, h):
    x1, y1 = x0 + w - 1, y0 + h - 1
    rect(d, x0 - 2, y0 - 2, x1 + 2, y1 + 2, WOOD_DARK)
    rect(d, x0, y0, x1, y1, NIGHT)
    rect(d, x0, y0, x1, y0 + h // 3, NIGHT_LIGHT)
    for sx, sy in ((x0 + 4, y0 + 5), (x1 - 6, y0 + 9), (x0 + w // 2 + 3, y1 - 6)):
        d.point([sx, sy], fill=STAR)
    mx, my = x0 + w // 2, y0 + h // 2
    d.line([mx, y0, mx, y1], fill=WOOD_DARK, width=2)
    d.line([x0, my, x1, my], fill=WOOD_DARK, width=2)
    rect(d, x0 - 3, y1 + 2, x1 + 3, y1 + 4, WOOD_LIGHT)  # sill


def lantern(d, cx, chain_top, chain_len):
    d.line([cx, chain_top, cx, chain_top + chain_len], fill=OUTLINE)
    top = chain_top + chain_len
    rect(d, cx - 3, top, cx + 3, top + 1, OUTLINE)
    rect(d, cx - 3, top + 2, cx + 3, top + 8, OUTLINE)
    rect(d, cx - 2, top + 2, cx + 2, top + 7, LAMP)
    rect(d, cx - 1, top + 3, cx, top + 5, LAMP_HOT)
    rect(d, cx - 2, top + 9, cx + 2, top + 9, OUTLINE)


def barrel(d, x0, y0, w, h):
    x1, y1 = x0 + w - 1, y0 + h - 1
    d.ellipse([x0, y0, x1, y1], fill=WOOD_MID, outline=OUTLINE)
    rect(d, x0 + 1, y0 + 3, x1 - 1, y1 - 3, WOOD_MID)
    d.line([x0, y0 + 3, x0, y1 - 3], fill=OUTLINE)
    d.line([x1, y0 + 3, x1, y1 - 3], fill=OUTLINE)
    for y in (y0 + 4, y1 - 5):
        rect(d, x0 + 1, y, x1 - 1, y + 1, STONE_DARK)
    d.line([x0 + 3, y0 + 5, x0 + 3, y1 - 5], fill=WOOD_LIGHT)


def crate(d, x0, y0, s):
    x1, y1 = x0 + s - 1, y0 + s - 1
    rect(d, x0, y0, x1, y1, OUTLINE)
    rect(d, x0 + 1, y0 + 1, x1 - 1, y1 - 1, WOOD_LIGHT)
    rect(d, x0 + 3, y0 + 3, x1 - 3, y1 - 3, WOOD_MID)
    d.line([x0 + 3, y0 + 3, x1 - 3, y1 - 3], fill=WOOD_LIGHT, width=2)


# --- Backgrounds (640x144, the size Background.csv crops) ---

def tavern():
    W, H, FLOOR_Y = 640, 144, 100
    img = new(W, H, PLASTER)
    d = ImageDraw.Draw(img)

    speckle(img, (0, 6, W - 1, FLOOR_Y - 22), [PLASTER_DARK, PLASTER_LIGHT], 0.06, seed=1)
    rect(d, 0, FLOOR_Y - 21, W - 1, FLOOR_Y - 1, WOOD_MID)  # wainscot
    rect(d, 0, FLOOR_Y - 21, W - 1, FLOOR_Y - 20, WOOD_LIGHT)
    for x in range(6, W, 12):
        d.line([x, FLOOR_Y - 18, x, FLOOR_Y - 2], fill=WOOD_DARK)

    planks(d, FLOOR_Y, H - 1, W, seed=2)
    rect(d, 0, FLOOR_Y - 1, W - 1, FLOOR_Y, OUTLINE)  # skirting shadow

    rect(d, 0, 0, W - 1, 5, WOOD_DARK)  # ceiling beam
    rect(d, 0, 6, W - 1, 6, OUTLINE)
    for x in (0, 104, 222, 344, 556, 634):
        rect(d, x, 6, x + 5, FLOOR_Y - 1, WOOD_DARK)
        d.line([x + 1, 6, x + 1, FLOOR_Y - 1], fill=WOOD_MID)

    window(d, 128, 24, 28, 32)
    window(d, 368, 24, 28, 32)

    # Fireplace
    rect(d, 236, 38, 302, FLOOR_Y - 1, STONE)
    for y in range(40, FLOOR_Y, 6):
        off = 0 if (y // 6) % 2 else 6
        d.line([236, y, 302, y], fill=STONE_DARK)
        for x in range(236 + off, 302, 12):
            d.line([x, y, x, min(y + 5, FLOOR_Y - 1)], fill=STONE_DARK)
    rect(d, 232, 34, 306, 38, STONE_LIGHT)  # mantel
    rect(d, 250, 64, 288, FLOOR_Y - 1, DOORWAY)
    d.polygon([(256, 99), (262, 80), (268, 90), (274, 74), (280, 92), (284, 99)], fill=FIRE_A)
    d.polygon([(262, 99), (268, 88), (274, 82), (279, 99)], fill=FIRE_B)
    rect(d, 254, 96, 286, 99, WOOD_DARK)  # logs

    # Bar counter with a bottle shelf behind it
    for shelf_y in (40, 58):
        rect(d, 420, shelf_y, 540, shelf_y + 2, WOOD_DARK)
        rnd = random.Random(shelf_y)
        x = 424
        while x < 534:
            h = rnd.randint(6, 10)
            col = rnd.choice([(60, 120, 60), (120, 40, 40), (150, 120, 60), (70, 70, 130)])
            rect(d, x, shelf_y - h, x + 3, shelf_y - 1, col)
            rect(d, x + 1, shelf_y - h - 2, x + 2, shelf_y - h - 1, col)
            d.point([x + 1, shelf_y - h + 2], fill=(230, 230, 210))
            x += rnd.randint(6, 10)
    rect(d, 414, 68, 546, 73, WOOD_HIGHLIGHT)  # counter top
    rect(d, 414, 74, 546, FLOOR_Y - 1, WOOD_DARK)
    for x in range(418, 546, 16):
        rect(d, x, 77, x + 11, FLOOR_Y - 4, WOOD_MID)

    barrel(d, 312, 80, 18, 22)
    barrel(d, 548, 82, 16, 20)

    for cx in (190, 330, 480):
        lantern(d, cx, 6, 12)

    # Doors: door_to_bar (left) and door_to_backroom (right), matching the
    # object rects in Objects.csv / scenes/tavern/GameObjects.csv.
    closed_door(d, 32, 52, 40, 48)
    closed_door(d, 594, 42, 26, 58)
    return img


def backroom():
    W, H, FLOOR_Y = 640, 144, 88
    img = new(W, H, STONE)
    d = ImageDraw.Draw(img)

    # Stone block wall
    for row, y in enumerate(range(0, FLOOR_Y, 8)):
        off = 0 if row % 2 else 10
        d.line([0, y, W - 1, y], fill=STONE_DARK)
        for x in range(-off, W, 20):
            d.line([x, y, x, y + 7], fill=STONE_DARK)
            d.line([x + 1, y + 1, x + 18, y + 1], fill=STONE_LIGHT)
    speckle(img, (0, 0, W - 1, FLOOR_Y - 1), [STONE_DARK, STONE_LIGHT], 0.03, seed=3)

    planks(d, FLOOR_Y, H - 1, W, seed=4)
    rect(d, 0, FLOOR_Y - 1, W - 1, FLOOR_Y, OUTLINE)

    rect(d, 0, 0, W - 1, 4, WOOD_DARK)
    for x in (0, 210, 400, 634):
        rect(d, x, 5, x + 5, FLOOR_Y - 1, WOOD_DARK)

    window(d, 300, 14, 22, 20)

    # Storage: crates on the left, barrels on the right, a shelf of jars.
    crate(d, 130, 64, 24)
    crate(d, 156, 64, 24)
    crate(d, 142, 40, 24)
    barrel(d, 440, 64, 22, 26)
    barrel(d, 464, 66, 20, 24)
    rect(d, 520, 30, 600, 32, WOOD_DARK)
    rect(d, 520, 52, 600, 54, WOOD_DARK)
    for i, x in enumerate(range(524, 596, 9)):
        for shelf_y in (30, 52):
            col = [(170, 150, 90), (120, 90, 60), (150, 160, 150)][(i + shelf_y) % 3]
            rect(d, x, shelf_y - 8, x + 5, shelf_y - 1, col)
            rect(d, x, shelf_y - 9, x + 5, shelf_y - 9, OUTLINE)

    lantern(d, 250, 5, 10)

    closed_door(d, 60, 40, 40, 48)  # door_to_bar
    return img


# --- Characters ---

def limb(d, x0, y0, x1, y1, color, width):
    d.line([x0, y0, x1, y1], fill=color, width=width)


def hero_walk():
    """6 frames of 32x47 facing right; frame 0 doubles as the idle pose."""
    FW, FH, FRAMES = 32, 47, 6
    sheet = new(FW * FRAMES, FH)
    HAIR = (92, 52, 28)
    TUNIC = (58, 128, 84)
    TUNIC_SHADE = (40, 96, 62)
    BELT = (80, 50, 26)
    PANTS = (70, 80, 112)
    PANTS_SHADE = (52, 60, 86)
    BOOTS = (44, 30, 22)

    for f in range(FRAMES):
        img = new(FW, FH)
        d = ImageDraw.Draw(img)
        p = 2 * math.pi * f / FRAMES
        stride = round(5 * math.sin(p))
        bob = 0 if abs(math.sin(p)) > 0.5 else -1
        cx, hip_y = 15, 28 + bob

        # Far side first (darker), then torso, then near side.
        limb(d, cx, hip_y, cx - stride, 42, PANTS_SHADE, 4)
        rect(d, cx - stride - 2, 42, cx - stride + 3, 45, BOOTS)
        limb(d, cx, 14 + bob, cx + stride // 2 + 1, 25 + bob, TUNIC_SHADE, 3)

        rect(d, cx - 5, 12 + bob, cx + 5, 27 + bob, TUNIC)
        rect(d, cx - 5, 12 + bob, cx - 4, 27 + bob, TUNIC_SHADE)
        rect(d, cx - 5, 25 + bob, cx + 5, 26 + bob, BELT)
        d.point([cx + 2, 25 + bob], fill=LAMP)

        limb(d, cx + 1, hip_y, cx + 1 + stride, 42, PANTS, 4)
        rect(d, cx + stride - 1, 42, cx + stride + 4, 45, BOOTS)
        limb(d, cx, 14 + bob, cx - stride // 2 - 1, 25 + bob, TUNIC, 3)
        rect(d, cx - stride // 2 - 2, 25 + bob, cx - stride // 2, 26 + bob, SKIN)

        # Head, facing right
        rect(d, cx - 1, 10 + bob, cx + 2, 11 + bob, SKIN_SHADE)  # neck
        rect(d, cx - 4, 2 + bob, cx + 4, 10 + bob, SKIN)
        rect(d, cx - 5, 1 + bob, cx + 3, 2 + bob, HAIR)
        rect(d, cx - 5, 1 + bob, cx - 2, 8 + bob, HAIR)
        d.point([cx + 5, 6 + bob], fill=SKIN)  # nose
        d.point([cx + 3, 5 + bob], fill=OUTLINE)  # eye
        d.line([cx + 2, 8 + bob, cx + 3, 8 + bob], fill=SKIN_SHADE)

        sheet.alpha_composite(img, (f * FW, 0))
    return sheet


def merchant():
    """37x57, a fast-talking salesman facing the player."""
    img = new(37, 57)
    d = ImageDraw.Draw(img)
    HAT = (70, 44, 30)
    HAT_BAND = (150, 40, 40)
    COAT = (188, 150, 52)
    COAT_SHADE = (150, 116, 36)
    SHIRT = (230, 226, 210)
    TROUSERS = (96, 96, 104)
    SHOES = (36, 26, 22)
    cx = 18

    rect(d, cx - 5, 0, cx + 5, 9, HAT)  # top hat
    rect(d, cx - 5, 7, cx + 5, 8, HAT_BAND)
    rect(d, cx - 9, 10, cx + 9, 11, HAT)

    rect(d, cx - 5, 12, cx + 5, 22, SKIN)
    rect(d, cx - 5, 12, cx - 4, 22, SKIN_SHADE)
    d.point([cx - 2, 15], fill=OUTLINE)
    d.point([cx + 2, 15], fill=OUTLINE)
    rect(d, cx - 3, 18, cx + 3, 18, OUTLINE)  # that grin
    rect(d, cx - 2, 19, cx + 2, 19, SHIRT)
    d.line([cx - 4, 17, cx - 3, 18], fill=OUTLINE)
    d.line([cx + 3, 18, cx + 4, 17], fill=OUTLINE)

    rect(d, cx - 8, 23, cx + 8, 42, COAT)
    rect(d, cx - 8, 23, cx - 6, 42, COAT_SHADE)
    d.polygon([(cx - 3, 23), (cx + 3, 23), (cx, 31)], fill=SHIRT)
    rect(d, cx - 2, 23, cx + 2, 24, HAT_BAND)  # bow tie
    for y in (33, 37):
        d.point([cx, y], fill=OUTLINE)

    limb(d, cx - 8, 25, cx - 12, 38, COAT_SHADE, 3)  # arm down
    rect(d, cx - 14, 38, cx - 11, 40, SKIN)
    limb(d, cx + 8, 25, cx + 14, 30, COAT, 3)  # arm raised, pitching
    limb(d, cx + 14, 30, cx + 15, 22, COAT, 3)
    rect(d, cx + 14, 19, cx + 17, 22, SKIN)

    rect(d, cx - 6, 43, cx - 1, 53, TROUSERS)
    rect(d, cx + 1, 43, cx + 6, 53, TROUSERS)
    for y in range(44, 53, 3):
        d.line([cx - 6, y, cx - 1, y], fill=COAT_SHADE)
        d.line([cx + 1, y, cx + 6, y], fill=COAT_SHADE)
    rect(d, cx - 8, 54, cx - 1, 56, SHOES)
    rect(d, cx + 1, 54, cx + 8, 56, SHOES)
    return img


def deckhand():
    """32x58 facing left (the right-facing idle is mirrored in code)."""
    img = new(32, 58)
    d = ImageDraw.Draw(img)
    BANDANA = (180, 40, 40)
    BEARD = (40, 28, 22)
    STRIPE_A = (230, 230, 220)
    STRIPE_B = (40, 56, 110)
    TROUSERS = (90, 70, 50)
    BOOTS = (34, 24, 20)
    cx = 16

    rect(d, cx - 5, 4, cx + 5, 16, SKIN)
    rect(d, cx + 4, 4, cx + 5, 16, SKIN_SHADE)
    rect(d, cx - 6, 2, cx + 6, 6, BANDANA)
    rect(d, cx + 6, 4, cx + 8, 8, BANDANA)  # knot at the back
    d.point([cx - 7, 10], fill=SKIN)  # nose
    d.point([cx - 3, 9], fill=OUTLINE)
    rect(d, cx - 6, 12, cx + 3, 17, BEARD)
    rect(d, cx - 4, 13, cx - 2, 13, (150, 80, 70))  # mouth

    for i, y in enumerate(range(18, 37, 2)):
        rect(d, cx - 7, y, cx + 7, y + 1, STRIPE_A if i % 2 else STRIPE_B)
    limb(d, cx - 7, 20, cx - 9, 33, STRIPE_B, 4)
    rect(d, cx - 11, 33, cx - 8, 36, SKIN)
    limb(d, cx + 7, 20, cx + 8, 33, STRIPE_B, 4)
    rect(d, cx + 7, 33, cx + 10, 36, SKIN_SHADE)

    rect(d, cx - 6, 37, cx - 1, 53, TROUSERS)
    rect(d, cx + 1, 37, cx + 6, 53, TROUSERS)
    rect(d, cx - 6, 37, cx + 6, 38, OUTLINE)  # belt
    rect(d, cx - 9, 54, cx - 1, 57, BOOTS)
    rect(d, cx + 1, 54, cx + 7, 57, BOOTS)
    return img


# --- Props ---

def table():
    img = new(84, 29)
    d = ImageDraw.Draw(img)
    for x in (12, 68):  # back legs
        rect(d, x, 7, x + 3, 23, WOOD_DARK)
    rect(d, 0, 0, 83, 4, WOOD_HIGHLIGHT)
    rect(d, 0, 5, 83, 8, WOOD_MID)
    d.line([0, 8, 83, 8], fill=OUTLINE)
    for x in range(10, 84, 21):
        d.line([x, 0, x, 4], fill=WOOD_LIGHT)
    for x in (3, 77):  # front legs
        rect(d, x, 9, x + 3, 28, WOOD_MID)
        d.line([x, 9, x, 28], fill=WOOD_LIGHT)
    return img


def table2():
    img = new(62, 26)
    d = ImageDraw.Draw(img)
    d.ellipse([18, 21, 43, 25], fill=WOOD_DARK)
    rect(d, 28, 8, 33, 22, WOOD_MID)
    d.line([29, 8, 29, 22], fill=WOOD_LIGHT)
    d.ellipse([0, 2, 61, 12], fill=WOOD_DARK)
    d.ellipse([0, 0, 61, 10], fill=WOOD_HIGHLIGHT, outline=WOOD_MID)
    return img


def mug(size):
    """Tin mug; 10x10 in the scene, 19x19 as the inventory icon."""
    s = size / 10
    img = new(size, size)
    d = ImageDraw.Draw(img)
    TIN = (168, 170, 180)
    TIN_DARK = (110, 112, 124)
    TIN_LIGHT = (214, 216, 224)
    d.ellipse([round(5 * s), round(3 * s), round(9 * s), round(7.5 * s)], outline=TIN_DARK, width=max(1, round(s)))
    rect(d, round(1 * s), round(2 * s), round(6.5 * s), size - 1, TIN)
    rect(d, round(1 * s), round(2 * s), round(1 * s) + max(0, round(s) - 1), size - 1, TIN_DARK)
    rect(d, round(5.5 * s), round(2 * s), round(6.5 * s), size - 1, TIN_DARK)
    rect(d, round(1 * s), round(1 * s), round(6.5 * s), round(2 * s), TIN_LIGHT)
    rect(d, round(2.5 * s), round(3.5 * s), round(2.5 * s), size - 2, TIN_LIGHT)
    if size > 12:
        d.point([round(4 * s), round(6 * s)], fill=TIN_DARK)  # the dent
    return img


def open_doorway(w, h):
    """Drawn over a door's rect when it's opened: the dark room beyond, with
    the leaf swung back against the hinge side."""
    img = new(w, h, DOORWAY + (255,))
    d = ImageDraw.Draw(img)
    leaf = max(3, w // 6)
    rect(d, 0, 0, leaf - 1, h - 1, WOOD_DARK)
    d.line([leaf - 1, 0, leaf - 1, h - 1], fill=WOOD_MID)
    rect(d, leaf + 2, h - 3, w - 3, h - 1, (52, 38, 28))  # light on the floor beyond
    return img


def cursor():
    img = new(17, 17)
    d = ImageDraw.Draw(img)
    white = (255, 255, 255)
    d.line([0, 8, 5, 8], fill=white)
    d.line([11, 8, 16, 8], fill=white)
    d.line([8, 0, 8, 5], fill=white)
    d.line([8, 11, 8, 16], fill=white)
    return img


def main():
    os.makedirs(OUT_DIR, exist_ok=True)
    print(f'Writing placeholder art to {OUT_DIR}')
    save(tavern(), 'tavern.png')
    save(backroom(), 'backroom.png')
    save(hero_walk(), 'hero_walk.png')
    save(merchant(), 'merchant.png')
    save(deckhand(), 'deckhand.png')
    save(table(), 'table.png')
    save(table2(), 'table2.png')
    save(mug(10), 'mug.png')
    save(mug(19), 'mug_item.png')
    save(open_doorway(26, 58), 'door.png')
    save(open_doorway(40, 48), 'bar_door.png')
    save(cursor(), 'cursor.png')


if __name__ == '__main__':
    main()
