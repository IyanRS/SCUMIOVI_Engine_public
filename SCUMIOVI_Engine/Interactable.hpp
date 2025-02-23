#pragma once

#include <map>
#include "Event.hpp"
#include "Verb.hpp"
#include <string>

#include "Vector2.hpp"


class Interactable {
	std::map<std::string, Event> interactions;
	//std::map<std::string, std::string> interactionsSelectedObjects;
	Verb* defaultVerb;
	Event mouseOverEvent;
	Event mouseNotOverEvent;

	Event mouseOverEventPlusHighlight;
	Event mouseNotOverEventPlusNoHighlight;

	bool shouldHighlightDefaultVerb;
public:
	bool onClick(Vector2<> clickPos, Verb* verb);
	void onMouseOver(Vector2<> mousePos);
	void onMouseNotOver(Vector2<> mousePos);
	bool interact(Vector2<> clickPos, Verb* verb);
	Verb* getDefaultVerb();
	void setDefaultVerb(Verb* verb);
	void setDefaultVerb(std::string verbName);
	void addVerbInteraction(Verb& verb, Event event);
	//void addVerbInteraction(Verb& verb, std::string selectedObjectName, Event event);
	void removeInteraction(Verb& verb);
	void removeInteraction(std::string verbName);
	Event& getMouseOverEvent();
	void setMouseOverEvent(Event event);
	Event& getMouseNotOverEvent();
	void setMouseNotOverEvent(Event event);
	bool hasVerbInteraction(Verb& verb);
};