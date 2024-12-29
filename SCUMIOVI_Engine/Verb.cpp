#include "Verb.hpp"

#include "Game.hpp"

Verb::Verb() {
	Game::getInstance().addVerb(*this); // TODO: maybe it shouldn't be added to the current player
}

Verb::Verb(std::string name) {
	this->name = name;
	Game::getInstance().addVerb(*this);
}

std::string& Verb::getName() {
	return name;
}