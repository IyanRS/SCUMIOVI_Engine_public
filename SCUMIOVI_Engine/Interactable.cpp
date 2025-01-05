#include "Interactable.hpp"

#include "Game.hpp"

bool Interactable::onClick(Vector2<> clickPos, Verb* verb) {
	Game& game = Game::getInstance();
	if (verb->getName() == "selected")
		return interact(clickPos, game.getCurrentPlayer().getVerb());
	if (verb->getName() == "default")
		return interact(clickPos, defaultVerb);
	return interact(clickPos, verb);
}

void Interactable::onMouseOver(Vector2<> mousePos) {
	Game& game = Game::getInstance();
	EventInfo e = EventInfo(mousePos, game.getCurrentPlayer(), *this);
	mouseOverEvent.execute(e);
}

void Interactable::onMouseNotOver(Vector2<> mousePos) {
	Game& game = Game::getInstance();
	EventInfo e = EventInfo(mousePos, game.getCurrentPlayer(), *this);
	mouseNotOverEvent.execute(e);
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
		Game& game = Game::getInstance();
		EventInfo e = EventInfo(clickPos, game.getCurrentPlayer(), *this);
		it->second.execute(e);
		// TODO: default before and after Events? (game.changeToDefaultVerb(); could be inside the event after)
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
			Game& game = Game::getInstance();
			EventInfo e = EventInfo(clickPos, game.getCurrentPlayer(), *this);
			it->second.execute(e);
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

Verb* Interactable::getDefaultVerb() {
	return defaultVerb;
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

Event& Interactable::getMouseOverEvent() {
	return mouseOverEvent;
}

void Interactable::setMouseOverEvent(Event event) {
	mouseOverEvent = event;
}

Event& Interactable::getMouseNotOverEvent() {
	return mouseNotOverEvent;
}

void Interactable::setMouseNotOverEvent(Event event) {
	mouseNotOverEvent = event;
}