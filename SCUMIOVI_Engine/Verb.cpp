#include "Verb.hpp"

#include "Game.hpp"

Verb::Verb() {
	Game::getInstance().getCurrentPlayer().addVerb(*this); // TODO: maybe it shouldn't be added to the current player
}

Verb::Verb(std::string name, bool showInUI) {
	this->name = name;
	this->showInUI = showInUI;
}

std::string& Verb::getName() {
	return name;
}