#include "Interactable.hpp"

#include "Game.hpp"

bool Interactable::onClick(Vector2<> clickPos) {
	//interact(Game::getInstance().getCurrentPlayer().getSelected)
	Game& game = Game::getInstance();
	return interact(clickPos, game.getCurrentPlayer().getSelectedVerb());
}

bool Interactable::interact(Vector2<> clickPos, Verb* verb) {
	if (verb == nullptr) {
		if (defaultVerb == nullptr) {
			return false;
		}
		verb = defaultVerb;
	}
	std::map<std::string, Event>::iterator it = interactions.find(defaultVerb->getName());
	if (it != interactions.end()) {
		it->second.execute(clickPos);
		return true;
	}
	return false;
}

void Interactable::setDefaultVerb(Verb* verb) {
	defaultVerb = verb;
}

void Interactable::addVerbInteraction(Verb& verb, Event event) {
	interactions[verb.getName()] = event;
}