#pragma once

#include <string>
#include "GameObject.hpp"

class Player : public GameObject {
	std::string name;
	bool canPlayerMove = true;
	Verb* selectedVerb;
public:
	using GameObject::GameObject;
	void enableMovement();
	void disableMovement();
	bool canMove();
	void setVerb(std::string verbName);
	void setVerb(Verb* verb);
	Verb* getVerb();
};