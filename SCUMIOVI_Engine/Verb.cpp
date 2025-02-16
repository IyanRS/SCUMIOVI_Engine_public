#include "Verb.hpp"

#include "Game.hpp"

// Special verbs that represent the default verb of the object that 
// we can interact with or any possible verb
Verb Verb::defaultVerb = Verb("default", "Default");
Verb Verb::anyVerb = Verb("any", "Any");
Verb Verb::selectedVerb = Verb("selected", "Selected");

Verb Verb::walkVerb = Verb("walk", "Walk to");
Verb Verb::pushVerb = Verb("push", "Push", VerbSyntax({ "Push", "Push $object0"}));
//Verb Verb::pushVerb = Verb("push", "Push");
Verb Verb::closeVerb = Verb("close", "Close");
Verb Verb::openVerb = Verb("open", "Open");
Verb Verb::talkVerb = Verb("talk", "Talk to");
Verb Verb::giveVerb = Verb("give", "Give");
Verb Verb::pickupVerb = Verb("pickup", "Pick up");
Verb Verb::useVerb = Verb("use", "Use");
Verb Verb::lookAtVerb = Verb("lookat", "Look at");
Verb Verb::pullVerb = Verb("pull", "Pull");

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

Verb::Verb(std::string name, std::string displayName, VerbSyntax syntax) {
	this->name = name;
	this->displayName = displayName;
	Game::getInstance().addVerb(this);
	this->verbSyntax = syntax;
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

std::string Verb::getInfo(std::vector<GameObject*> gameObjects) {
	if (verbSyntax.getStringsCount() == 0)
		return displayName;
	return verbSyntax.getInfo(gameObjects);
}