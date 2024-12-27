#pragma once

#include <string>
#include "GameObject.hpp"

class Player : public GameObject {
	std::string name;
	bool canPlayerMove = true;
	std::list<Verb> verbs;
	Verb* selectedVerb;
public:
	using GameObject::GameObject;
	void enableMovement();
	void disableMovement();
	bool canMove();
	Verb* getVerb(std::string verbName);
	void setVerb(std::string verbName);
	void setVerb(Verb* verb);
	void addVerb(Verb& verb);
	Verb* getSelectedVerb();
};