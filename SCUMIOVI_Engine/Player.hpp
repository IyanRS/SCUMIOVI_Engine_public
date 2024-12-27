#pragma once

#include <string>
#include "GameObject.hpp"

class Player : public GameObject {
	std::string name;
	bool canPlayerMove = true;
public:
	using GameObject::GameObject;
	void enableMovement();
	void disableMovement();
	bool canMove();
};