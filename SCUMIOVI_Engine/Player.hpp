#pragma once

#include <string>
#include "GameObject.hpp"
#include "Inventory.hpp"

class Player : public GameObject {
	std::string name;
	bool canPlayerMove = true;
	Verb* selectedVerb;
	GameObject* selectedObject;
	GameObject* targetObject;
	Inventory inventory;
public:
	using GameObject::GameObject;
	void enableMovement();
	void disableMovement();
	bool canMove();
	void setVerb(std::string verbName);
	void setVerb(Verb* verb);
	Verb* getVerb();
	Inventory& getInventory();
	GameObject* getSelectedObject();
	void setSelectedObject(GameObject* gameObject);
	void setTargetObject(GameObject* gameObject);
	GameObject* getTargetObject();
	//void moveTo(Vector2<> pos, float seconds, Event callback);
};