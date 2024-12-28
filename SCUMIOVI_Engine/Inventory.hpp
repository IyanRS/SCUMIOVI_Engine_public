#pragma once

#include <list>

#include "GameObject.hpp"
#include "Verb.hpp"

class Inventory {
	std::list<GameObject> items;
	GameObject selectedItem;
};