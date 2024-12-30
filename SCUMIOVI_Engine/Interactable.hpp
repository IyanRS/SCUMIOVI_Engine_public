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
	bool onClick(Vector2<> clickPos, Verb* verb);
	bool interact(Vector2<> clickPos, Verb* verb);
	void setDefaultVerb(Verb* verb);
	void setDefaultVerb(std::string verbName);
	void addVerbInteraction(Verb& verb, Event event);
	void removeInteraction(Verb& verb);
	void removeInteraction(std::string verbName);
};