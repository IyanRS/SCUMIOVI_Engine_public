#pragma once

#include "Vector2.hpp"

struct CameraZone {
    float minX;
    float maxX;
    Vector2<> cameraTarget;
};
