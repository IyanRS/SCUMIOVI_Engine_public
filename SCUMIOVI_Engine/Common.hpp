#pragma once

#define RENDER_WIDTH 320
#define RENDER_HEIGHT 200
#define WINDOW_WIDTH (320*2)
#define WINDOW_HEIGHT (200*2)

//TODO: Make a class that allows modifying configuration to allow dynamic changes

#define FPS 60

// Shared floating-point tolerance for position/time comparisons.
constexpr float EPSILON = 0.001f;

#define PIXEL_PERFECT
#define PIXEL_PERFECT_CURSOR