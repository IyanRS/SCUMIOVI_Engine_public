#pragma once

#include <string>
#include "GameObject.hpp"
#include "Inventory.hpp"

class Player : public GameObject {
	std::string name;
	bool canPlayerMove = true;
	std::shared_ptr<Verb> selectedVerb = nullptr;
	std::weak_ptr<GameObject> selectedObject;
	std::weak_ptr<GameObject> targetObject;
	Inventory inventory;
public:
	using GameObject::GameObject;
	void enableMovement();
	void disableMovement();
	bool canMove();
	void setVerb(std::string verbName);
	void setVerb(std::shared_ptr<Verb> verb);
	std::shared_ptr<Verb> getVerb();
	Inventory& getInventory();
	std::shared_ptr<GameObject> getSelectedObject();
	void setSelectedObject(std::shared_ptr<GameObject> gameObject);
	void setTargetObject(std::shared_ptr<GameObject> gameObject);
	std::shared_ptr<GameObject> getTargetObject();
	//void moveTo(Vector2<> pos, float seconds, Event callback);
};