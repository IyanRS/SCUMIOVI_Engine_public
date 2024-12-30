#include "Verb.hpp"

#include "Game.hpp"

Verb::Verb() {
	Game::getInstance().addVerb(this);
}

Verb::Verb(std::string name) {
	this->name = name;
	Game::getInstance().addVerb(this);
}

std::string& Verb::getName() {
	return name;
}

/// <summary>
/// Sets the default event that will always be executed before the object interaction when using the verb
/// </summary>
/// <param name="event"></param>
void Verb::setDefaultEvent(Event* event) {
	defaultEvent = event;
}

Event* Verb::getDefaultEvent() {
	return defaultEvent;
}