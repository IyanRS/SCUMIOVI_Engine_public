#include "Interactable.hpp"

#include "Game.hpp"

bool Interactable::onClick(Vector2<> clickPos, Verb* verb) {
	Game& game = Game::getInstance();
	if (verb->getName() == Verb::selectedVerb.getName())
		return interact(clickPos, game.getCurrentPlayer().getVerb());
	if (verb->getName() == Verb::defaultVerb.getName())
		if (defaultVerb == nullptr)
			return interact(clickPos, game.getDefaultVerb());
		else
			return interact(clickPos, defaultVerb);
	return interact(clickPos, verb);
}

void Interactable::onMouseOver(Vector2<> mousePos) {
	Game& game = Game::getInstance();
	EventInfo e = EventInfo(mousePos, game.getCurrentPlayer(), *this);
	getMouseOverEvent().execute(e);
}

void Interactable::onMouseNotOver(Vector2<> mousePos) {
	Game& game = Game::getInstance();
	EventInfo e = EventInfo(mousePos, game.getCurrentPlayer(), *this);
	getMouseNotOverEvent().execute(e);
}

bool Interactable::interact(Vector2<> clickPos, Verb* verb) {
	if (verb == nullptr) {
		if (defaultVerb == nullptr) {
			return false;
		}
		verb = defaultVerb;
	}

	Game& game = Game::getInstance();
	EventInfo e = EventInfo(clickPos, game.getCurrentPlayer(), *this);

	std::map<std::string, Event>::iterator it = interactions.find(verb->getName());
	if (it != interactions.end()) {
		Event* beforeActualEvent = verb->getBeforeActualEvent();
		Event* afterActualEvent = verb->getAfterActualEvent();

		if (verb->RequiresSelectedObject()) {
			if (game.getCurrentPlayer().getSelectedObject() == nullptr)
				game.getCurrentPlayer().setSelectedObject((GameObject*)this);
			else {
				std::map<std::string, std::string>::iterator it1 = interactionsSelectedObjects.find(verb->getName());
				if (it1 != interactionsSelectedObjects.end()) {
					if (game.getCurrentPlayer().getSelectedObject()->getName() == it1->second) {
						if (beforeActualEvent != nullptr) {
							beforeActualEvent->execute(e);
						}
						it->second.execute();
						if (afterActualEvent != nullptr) {
							afterActualEvent->execute(e);
						}
						game.changeToDefaultVerb();
						game.getCurrentPlayer().setSelectedObject(nullptr);
					}
				}
			}
		}
		else {
			if (beforeActualEvent != nullptr) {
				beforeActualEvent->execute(e);
			}
			it->second.execute(e);
			if (afterActualEvent != nullptr) {
				afterActualEvent->execute(e);
			}
			// TODO: default before and after Events? (game.changeToDefaultVerb(); could be inside the event after)
			game.changeToDefaultVerb();
			game.getCurrentPlayer().setSelectedObject(nullptr);
		}
		
		
		//game.getUI().updateUI();
		return true;
	}
	// TODO: handle invalid verbs, allow characters to say predefined lines such as "I can't do that".
	else {
		//if (verb)
		//if (defaultVerb == nullptr) {
		//	return false;
		//}
		it = interactions.find(Verb::anyVerb.getName());
		if (it != interactions.end()) {
			Game& game = Game::getInstance();
			EventInfo e = EventInfo(clickPos, game.getCurrentPlayer(), *this);
			it->second.execute(e);
			return true;
		}
	}

	Event* defaultEvent = verb->getDefaultEvent();
	if (defaultEvent != nullptr) {
		defaultEvent->execute(e);
		return true;
	}

	game.changeToDefaultVerb();
	game.getCurrentPlayer().setSelectedObject(nullptr);
	return false;
}

void Interactable::setDefaultVerb(Verb* verb) {
	defaultVerb = verb;
	//shouldHighlightDefaultVerb = highlight;
}

Verb* Interactable::getDefaultVerb() {
	return defaultVerb;
}

void Interactable::setDefaultVerb(std::string verbName) {
	Game& game = Game::getInstance();
	defaultVerb = game.getVerb(verbName);
	//shouldHighlightDefaultVerb = highlight;
}

void Interactable::addVerbInteraction(Verb& verb, Event event) {
	interactions[verb.getName()] = event;
}

void Interactable::addVerbInteraction(Verb& verb, std::string selectedObjectName, Event event) {
	addVerbInteraction(verb, event);
	interactionsSelectedObjects[verb.getName()] = selectedObjectName;
}

void Interactable::removeInteraction(Verb& verb) {
	interactions.erase(verb.getName());
}

void Interactable::removeInteraction(std::string verbName) {
	interactions.erase(verbName);
}

Event& Interactable::getMouseOverEvent() {
	if (defaultVerb && defaultVerb->getName() != Verb::anyVerb.getName()) {
		Event highlightDefaultVerbEvent = Event([this](EventInfo e) {
			e.game.getUI().setHighlightedVerb(getDefaultVerb());
			if (e.interactable.hasVerbInteraction(*e.player.getVerb()))
				e.player.setTargetObject((GameObject*)&e.interactable);
			e.game.getUI().updateUI();
		});

		mouseOverEventPlusHighlight = mouseOverEvent + highlightDefaultVerbEvent;
		return mouseOverEventPlusHighlight;
	}
	return mouseOverEvent;
}

void Interactable::setMouseOverEvent(Event event) {
	mouseOverEvent = event;
}

Event& Interactable::getMouseNotOverEvent() {
	if (defaultVerb && defaultVerb->getName() != Verb::anyVerb.getName()) {
		Event dontHighlightDefaultVerbEvent = Event([this](EventInfo e) {
			e.game.getUI().setHighlightedVerb(nullptr);
			e.player.setTargetObject(nullptr);
			e.game.getUI().updateUI();
		});

		mouseNotOverEventPlusNoHighlight = mouseOverEvent + dontHighlightDefaultVerbEvent;
		return mouseNotOverEventPlusNoHighlight;
	}
	return mouseNotOverEvent;
}

void Interactable::setMouseNotOverEvent(Event event) {
	mouseNotOverEvent = event;
}

bool Interactable::hasVerbInteraction(Verb& verb) {
	return interactions.find(verb.getName()) != interactions.end();
}