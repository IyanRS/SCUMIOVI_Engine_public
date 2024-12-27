#pragma once

#include "Scene.hpp"

class InputManager {
public:
	bool click(Scene& scene);
	Vector2<> getClickPosition(Camera&);
};