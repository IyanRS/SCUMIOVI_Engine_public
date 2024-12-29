#include "Interactable.hpp"

#include "Game.hpp"

bool Interactable::onClick(Vector2<> clickPos) {
	//interact(Game::getInstance().getCurrentPlayer().getSelected)
	Game& game = Game::getInstance();
	return interact(clickPos, game.getCurrentPlayer().getVerb());
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
		it->second.execute(clickPos);
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
	return false;
}

void Interactable::setDefaultVerb(Verb* verb) {
	defaultVerb = verb;
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