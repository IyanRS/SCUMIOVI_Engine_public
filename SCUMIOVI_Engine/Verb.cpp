#include "Verb.hpp"

#include "Game.hpp"

// Special verbs that represent the default verb of the object that 
// we can interact with or any possible verb
Verb Verb::defaultVerb = Verb("default", "Default");
Verb Verb::anyVerb = Verb("any", "Any");

Verb Verb::walkVerb = Verb("walk", "Walk to");
Verb Verb::pushVerb = Verb("push", "Push");
Verb Verb::closeVerb = Verb("close", "Close");
Verb Verb::openVerb = Verb("open", "Open");

Verb::Verb() {
	Game::getInstance().addVerb(this);
}

Verb::Verb(std::string name) {
	this->name = name;
	this->displayName = name;
	Game::getInstance().addVerb(this);
}

Verb::Verb(std::string name, std::string displayName) {
	this->name = name;
	this->displayName = displayName;
	Game::getInstance().addVerb(this);
}

std::string& Verb::getDisplayName() {
	return displayName;
}

std::string& Verb::getName() {
	return name;
}

void Verb::setDefaultEvent(Event* event) {
	defaultEvent = event;
}

Event* Verb::getDefaultEvent() {
	return defaultEvent;
}