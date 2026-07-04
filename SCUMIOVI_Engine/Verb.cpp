#include "Verb.hpp"

#include "Game.hpp"
#include "Localization.hpp"

void Verb::setDefaultDefaultEvent(const Event& event) {
	defaultDefaultEvent = event;
}

Verb::Verb() : requiresSelectedObject(false) {
	defaultEvent = defaultDefaultEvent;
}

Verb::Verb(std::string name) : requiresSelectedObject(false) {
	this->name = name;
	this->displayName = name;
	defaultEvent = defaultDefaultEvent;
}

Verb::Verb(std::string name, std::string displayName) : requiresSelectedObject(false) {
	this->name = name;
	this->displayName = displayName;
	defaultEvent = defaultDefaultEvent;
}

Verb::Verb(std::string name, std::string displayName, VerbSyntax syntax, bool requiresSelectedObject) {
	this->name = name;
	this->displayName = displayName;
	this->verbSyntax = syntax;
	this->requiresSelectedObject = requiresSelectedObject;
	defaultEvent = defaultDefaultEvent;
}

std::string Verb::getDisplayName() {
	return tr(displayName);
}

std::string& Verb::getName() {
	return name;
}

void Verb::setDefaultEvent(const Event& event) {
	defaultEvent = event;
}

Event Verb::getDefaultEvent() {
	return defaultEvent;
}

void Verb::setBeforeActualEvent(const Event& event) {
	beforeActualEvent = event;
}

Event Verb::getBeforeActualEvent() {
	return beforeActualEvent;
}

void Verb::setAfterActualEvent(const Event& event) {
	afterActualEvent = event;
}

Event Verb::getAfterActualEvent() {
	return afterActualEvent;
}

std::string Verb::getInfo(std::vector<std::shared_ptr<GameObject>> gameObjects) {
	if (verbSyntax.getStringsCount() == 0)
		return getDisplayName();
	return verbSyntax.getInfo(gameObjects);
}

std::string Verb::getInfo(std::shared_ptr<GameObject> selected, std::shared_ptr<GameObject> target) {
	if (verbSyntax.getStringsCount() == 0)
		return getDisplayName();
 	return verbSyntax.getInfo(selected, target);
}

bool Verb::RequiresSelectedObject() {
	return requiresSelectedObject;
}