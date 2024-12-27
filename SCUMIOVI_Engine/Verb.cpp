#include "Verb.hpp"

#include "Game.hpp"

Verb::Verb() {
	Game::getInstance().addVerb(*this);
}

Verb::Verb(std::string name, bool showInUI) {
	this->name = name;
	this->showInUI = showInUI;
}

std::string& Verb::getName() {
	return name;
}