#pragma once

#include <map>
#include "Event.hpp"
#include "Verb.hpp"
#include <string>

#include "Vector2.hpp"

class Interactable {
	std::map<std::string, Event> interactions;
	Verb* defaultVerb;
public:
	bool onClick(Vector2<> clickPos);
	bool interact(Vector2<> clickPos, Verb* verb);
	void setDefaultVerb(Verb* verb);
	void addInteraction(Verb& verb, Event event);
};