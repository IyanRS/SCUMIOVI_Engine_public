#include "Interactable.hpp"

#include "Game.hpp"

bool Interactable::onClick(Vector2<> clickPos, std::shared_ptr<Verb> verb) {
	Game& game = Game::getInstance();
	if (verb->getName() == "selected")
		return interact(clickPos, game.getCurrentPlayer()->getVerb());
	if (verb->getName() == "default")
		if (defaultVerb == nullptr)
			return interact(clickPos, game.getDefaultVerb());
		else
			return interact(clickPos, defaultVerb);
	return interact(clickPos, verb);
}

void Interactable::onMouseOver(Vector2<> mousePos) {
	Game& game = Game::getInstance();
	EventInfo e = EventInfo(mousePos, *game.getCurrentPlayer(), *this);
	getMouseOverEvent().execute(e);
}

void Interactable::onMouseNotOver(Vector2<> mousePos) {
	Game& game = Game::getInstance();
	EventInfo e = EventInfo(mousePos, *game.getCurrentPlayer(), *this);
	getMouseNotOverEvent().execute(e);
}

bool Interactable::interact(Vector2<> clickPos, std::shared_ptr<Verb> verb) {
	if (verb == nullptr) {
		if (defaultVerb == nullptr) {
			return false;
		}
		verb = defaultVerb;
	}

	Game& game = Game::getInstance();
	EventInfo e = EventInfo(clickPos, *game.getCurrentPlayer(), *this);

	auto it = interactions.find(verb->getName());
	if (it != interactions.end()) {
		if (verb->RequiresSelectedObject()) {
			if (game.getCurrentPlayer()->getSelectedObject() == nullptr) {
				GameObject* go = dynamic_cast<GameObject*>(this);
				if (go)
					game.getCurrentPlayer()->setSelectedObject(go->shared_from_this());
			}
			else {
				auto it1 = interactionsSelectedObjects.find(verb->getName());
				if (it1 != interactionsSelectedObjects.end()) {
					if (game.getCurrentPlayer()->getSelectedObject()->getName() == it1->second) {
						verb->getBeforeActualEvent().execute(e);
						it->second.execute(e);
						verb->getAfterActualEvent().execute(e);
						game.changeToDefaultVerb();
						game.getCurrentPlayer()->setSelectedObject(nullptr);
					}
				}
			}
		}
		else {
			verb->getBeforeActualEvent().execute(e);
			it->second.execute(e);
			verb->getAfterActualEvent().execute(e);
			game.changeToDefaultVerb();
			game.getCurrentPlayer()->setSelectedObject(nullptr);
		}
		
		return true;
	}
	else {
		it = interactions.find("any");
		if (it != interactions.end()) {
			it->second.execute(e);
			return true;
		}
	}

	// Combination registry: useVerb with selected object
	if (verb && verb->getName() == "use" && verb->RequiresSelectedObject()) {
		auto selected = game.getCurrentPlayer()->getSelectedObject();
		if (selected) {
			GameObject* go = dynamic_cast<GameObject*>(this);
			if (go) {
				Event* combo = game.findCombination(selected->getName(), go->getName());
				if (combo) {
					verb->getBeforeActualEvent().execute(e);
					combo->execute(e);
					verb->getAfterActualEvent().execute(e);
					game.changeToDefaultVerb();
					game.getCurrentPlayer()->setSelectedObject(nullptr);
					return true;
				}
			}
		}
	}

	verb->getDefaultEvent().execute(e);
	game.changeToDefaultVerb();
	game.getCurrentPlayer()->setSelectedObject(nullptr);
	return false;
}

void Interactable::setDefaultVerb(std::shared_ptr<Verb> verb) {
	defaultVerb = verb;
}

std::shared_ptr<Verb> Interactable::getDefaultVerb() {
	return defaultVerb;
}

void Interactable::setDefaultVerb(std::string verbName) {
	Game& game = Game::getInstance();
	defaultVerb = game.getVerb(verbName);
}

void Interactable::addVerbInteraction(std::shared_ptr<Verb> verb, Event event) {
	interactions[verb->getName()] = event;
}

void Interactable::addVerbInteraction(std::shared_ptr<Verb> verb, std::string selectedObjectName, Event event) {
	addVerbInteraction(verb, event);
	interactionsSelectedObjects[verb->getName()] = selectedObjectName;
}

void Interactable::removeInteraction(std::shared_ptr<Verb> verb) {
	interactions.erase(verb->getName());
}

void Interactable::removeInteraction(std::string verbName) {
	interactions.erase(verbName);
}

Event& Interactable::getMouseOverEvent() {
	Event hoverInfoEvent = Event([this](EventInfo e) {
		// Scene objects only. UI elements are GameObjects too, but they own
		// their hover visuals (a verb button highlights its own text) — a
		// blanket updateUI() from here would repaint the layout from the
		// highlighted-verb state and immediately undo them.
		GameObject* go = dynamic_cast<GameObject*>(&e.interactable);
		if (!go || go->getScene() == nullptr) return;

		// Naming the hovered object in the sentence line ("Talk to Stan")
		// must not depend on it having a default verb or an interaction for
		// the current verb — unmatched verbs fall back to their default
		// message when clicked, so the label should still name the object.
		// Scenery (backgrounds, decorations — see
		// GameObject::setInteractable) stays unnamed, and the player doesn't
		// name themselves. setTargetObject refreshes the UI itself.
		if (go->isInteractable() && go != e.game.getCurrentPlayer().get()) {
			e.player.setTargetObject(go->shared_from_this());
		}

		if (defaultVerb && defaultVerb->getName() != "any") {
			e.game.getUI().setHighlightedVerb(getDefaultVerb());
			e.game.getUI().updateUI();
		}
	});

	mouseOverEventPlusHighlight = mouseOverEvent + hoverInfoEvent;
	return mouseOverEventPlusHighlight;
}

void Interactable::setMouseOverEvent(Event event) {
	mouseOverEvent = event;
}

Event& Interactable::getMouseNotOverEvent() {
	Event clearInfoEvent = Event([this](EventInfo e) {
		// Scene objects only — see getMouseOverEvent.
		GameObject* go = dynamic_cast<GameObject*>(&e.interactable);
		if (!go || go->getScene() == nullptr) return;

		// Only clear the target if it's still this object — over/not-over
		// events for different objects can fire in the same frame (pointer
		// sliding from one object straight onto another), and this object's
		// exit must not wipe the target the other object just set.
		if (e.player.getTargetObject().get() == go) {
			e.player.setTargetObject(nullptr);
		}

		if (defaultVerb && defaultVerb->getName() != "any") {
			e.game.getUI().setHighlightedVerb(nullptr);
			e.game.getUI().updateUI();
		}
	});

	mouseNotOverEventPlusNoHighlight = mouseNotOverEvent + clearInfoEvent;
	return mouseNotOverEventPlusNoHighlight;
}

void Interactable::setMouseNotOverEvent(Event event) {
	mouseNotOverEvent = event;
}

bool Interactable::hasVerbInteraction(std::shared_ptr<Verb> verb) {
	return interactions.find(verb->getName()) != interactions.end();
}