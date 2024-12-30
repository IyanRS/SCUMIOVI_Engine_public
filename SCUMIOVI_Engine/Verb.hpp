#pragma once

#include <string>
#include "Event.hpp"

class Verb {
	std::string name;
	Event* defaultEvent;
public:
	Verb();
	Verb(std::string name);
	std::string& getName();
	void setDefaultEvent(Event* event);
	Event* getDefaultEvent();
};