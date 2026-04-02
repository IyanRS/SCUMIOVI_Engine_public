#pragma once

#include <map>
#include "Event.hpp"
#include "Verb.hpp"
#include <string>
#include <memory>

#include "Vector2.hpp"


class Interactable {
	std::map<std::string, Event> interactions;
	std::map<std::string, std::string> interactionsSelectedObjects;
	std::shared_ptr<Verb> defaultVerb = nullptr;
	Event mouseOverEvent;
	Event mouseNotOverEvent;

	Event mouseOverEventPlusHighlight;
	Event mouseNotOverEventPlusNoHighlight;

	bool shouldHighlightDefaultVerb;
public:
	bool onClick(Vector2<> clickPos, std::shared_ptr<Verb> verb);
	void onMouseOver(Vector2<> mousePos);
	void onMouseNotOver(Vector2<> mousePos);
	bool interact(Vector2<> clickPos, std::shared_ptr<Verb> verb);
	std::shared_ptr<Verb> getDefaultVerb();
	void setDefaultVerb(std::shared_ptr<Verb> verb);
	void setDefaultVerb(std::string verbName);
	void addVerbInteraction(std::shared_ptr<Verb> verb, Event event);
	void addVerbInteraction(std::shared_ptr<Verb> verb, std::string selectedObjectName, Event event);
	void removeInteraction(std::shared_ptr<Verb> verb);
	void removeInteraction(std::string verbName);
	Event& getMouseOverEvent();
	void setMouseOverEvent(Event event);
	Event& getMouseNotOverEvent();
	void setMouseNotOverEvent(Event event);
	bool hasVerbInteraction(std::shared_ptr<Verb> verb);
	virtual ~Interactable() = default;
};