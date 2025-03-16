#include "Verb.hpp"

#include "Game.hpp"

// Special verbs that represent the default verb of the object that 
// we can interact with or any possible verb
Verb Verb::defaultVerb = Verb("default", "Default");
Verb Verb::anyVerb = Verb("any", "Any");
Verb Verb::selectedVerb = Verb("selected", "Selected");

Verb Verb::walkVerb = Verb("walk", "Walk to");
Verb Verb::pushVerb = Verb("push", "Push", VerbSyntax({ "Push", "Push $target"}));
//Verb Verb::pushVerb = Verb("push", "Push");
Verb Verb::closeVerb = Verb("close", "Close");
Verb Verb::openVerb = Verb("open", "Open");
Verb Verb::talkVerb = Verb("talk", "Talk to");
Verb Verb::giveVerb = Verb("give", "Give", VerbSyntax({ "Give", "Give $target", "Give $selected to", "Give $selected to $target" }), true);
Verb Verb::pickupVerb = Verb("pickup", "Pick up");
Verb Verb::useVerb = Verb("use", "Use");
Verb Verb::lookAtVerb = Verb("lookat", "Look at");
Verb Verb::pullVerb = Verb("pull", "Pull");

Verb::Verb() {
	Game::getInstance().addVerb(this);
	defaultEvent = defaultDefaultEvent;
}

Verb::Verb(std::string name) {
	this->name = name;
	this->displayName = name;
	Game::getInstance().addVerb(this);
	defaultEvent = defaultDefaultEvent;
}

Verb::Verb(std::string name, std::string displayName) {
	this->name = name;
	this->displayName = displayName;
	Game::getInstance().addVerb(this);
	defaultEvent = defaultDefaultEvent;
}

Verb::Verb(std::string name, std::string displayName, VerbSyntax syntax, bool requiresSelectedObject) {
	this->name = name;
	this->displayName = displayName;
	Game::getInstance().addVerb(this);
	this->verbSyntax = syntax;
	this->requiresSelectedObject = requiresSelectedObject;
	defaultEvent = defaultDefaultEvent;
}

std::string& Verb::getDisplayName() {
	return displayName;
}

std::string& Verb::getName() {
	return name;
}

void Verb::setDefaultDefaultEvent(Event* event) {
	defaultDefaultEvent = event;
}

void Verb::setDefaultEvent(Event* event) {
	defaultEvent = event;
}

Event* Verb::getDefaultEvent() {
	return defaultEvent;
}


void Verb::setBeforeActualEvent(Event* event) {
	beforeActualEvent = event;
}

Event* Verb::getBeforeActualEvent() {
	return beforeActualEvent;
}


void Verb::setAfterActualEvent(Event* event) {
	afterActualEvent = event;
}

Event* Verb::getAfterActualEvent() {
	return afterActualEvent;
}

std::string Verb::getInfo(std::vector<GameObject*> gameObjects) {
	if (verbSyntax.getStringsCount() == 0)
		return displayName;
	return verbSyntax.getInfo(gameObjects);
}

std::string Verb::getInfo(GameObject* selected, GameObject* target) {
	if (verbSyntax.getStringsCount() == 0)
		return displayName;
 	return verbSyntax.getInfo(selected, target);
}

bool Verb::RequiresSelectedObject() {
	return requiresSelectedObject;
}