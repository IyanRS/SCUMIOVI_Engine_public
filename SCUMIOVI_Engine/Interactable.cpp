#include "Interactable.hpp"

#include "Game.hpp"

bool Interactable::onClick(Vector2<> clickPos, Verb* verb) {
	Game& game = Game::getInstance();
	if (verb == nullptr)
		return interact(clickPos, game.getCurrentPlayer().getVerb());
	return interact(clickPos, verb);
}

bool Interactable::interact(Vector2<> clickPos, Verb* verb) {
	if (verb == nullptr) {
		if (defaultVerb == nullptr) {
			return false;
		}
		verb = defaultVerb;
	}
	std::map<std::string, Event>::iterator it = interactions.find(verb->getName());
	if (it != interactions.end()) {
		Event* defaultEvent = verb->getDefaultEvent();
		if (defaultEvent != nullptr) {
			defaultEvent->execute();
		}
		it->second.execute(clickPos);
		Game& game = Game::getInstance();
		game.changeToDefaultVerb();
		return true;
	}
	// TODO: handle invalid verbs, allow characters to say predefined lines such as "I can't do that".
	else {
		if (defaultVerb == nullptr) {
			return false;
		}
		it = interactions.find("any");
		if (it != interactions.end()) {
			it->second.execute(clickPos);
			return true;
		}
	}
	Game& game = Game::getInstance();
	game.changeToDefaultVerb();
	return false;
}

void Interactable::setDefaultVerb(Verb* verb) {
	defaultVerb = verb;
}

void Interactable::setDefaultVerb(std::string verbName) {
	Game& game = Game::getInstance();
	defaultVerb = game.getVerb(verbName);
}

void Interactable::addVerbInteraction(Verb& verb, Event event) {
	interactions[verb.getName()] = event;
}

void Interactable::removeInteraction(Verb& verb) {
	interactions.erase(verb.getName());
}

void Interactable::removeInteraction(std::string verbName) {
	interactions.erase(verbName);
}