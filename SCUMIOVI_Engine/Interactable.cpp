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
	auto player = game.getCurrentPlayer();
	EventInfo e = EventInfo(clickPos, *player, *this);
	GameObject* go = dynamic_cast<GameObject*>(this);

	// --- Two-object verbs ("Use X on Y", "Give X to Y") ---------------------
	// The first click only picks the source object and builds the sentence
	// ("Use mug on ..."); the actual interaction resolves on the second click.
	// This is deliberately independent of whether the object has its own
	// interaction for the verb — any interactable object can be the source of
	// a two-object verb, which is what makes "use mug on table" reachable at
	// all. Scenery (see GameObject::setInteractable) can't be picked up as a
	// source, so clicking the background still falls through to the verb's
	// default response.
	if (verb->RequiresSelectedObject() && go && go->isInteractable()
		&& player->getSelectedObject() == nullptr) {
		player->setSelectedObject(go->shared_from_this());
		return true;
	}

	auto it = interactions.find(verb->getName());
	if (it != interactions.end()) {
		// A two-object verb only resolves against the object it was authored
		// for (Interactions.csv's other_object_id); a mismatch falls through
		// to the verb's default response.
		bool matchesSelection = true;
		if (verb->RequiresSelectedObject()) {
			auto it1 = interactionsSelectedObjects.find(verb->getName());
			auto selected = player->getSelectedObject();
			matchesSelection = it1 != interactionsSelectedObjects.end()
				&& selected != nullptr
				&& selected->getName() == it1->second;
		}

		if (matchesSelection) {
			verb->getBeforeActualEvent().execute(e);
			it->second.execute(e);
			verb->getAfterActualEvent().execute(e);
			game.changeToDefaultVerb();
			player->setSelectedObject(nullptr);
			return true;
		}
	}
	else {
		it = interactions.find("any");
		if (it != interactions.end()) {
			it->second.execute(e);
			return true;
		}
	}

	// Clicking the source object again, with nothing authored for that pair,
	// cancels the selection instead of scolding the player — a mis-click
	// shouldn't strand them mid-sentence. Checked after the lookups above so
	// it can never shadow an authored interaction.
	if (verb->RequiresSelectedObject() && go && player->getSelectedObject().get() == go) {
		player->setSelectedObject(nullptr);
		game.changeToDefaultVerb();
		return true;
	}

	// No matching interaction: the verb's default response ("I can't do
	// that") handles the click, so the click must be consumed. Returning
	// false here would let the still-running click loop pass the click to
	// whatever sits underneath (usually the background) — which re-resolves
	// the just-reset current verb and walks the player mid-message. The verb
	// reset means the NEXT click walks, matching the SCUMM feel.
	Event defaultEvent = verb->getDefaultEvent();
	bool handled = defaultEvent.hasActions();
	defaultEvent.execute(e);
	game.changeToDefaultVerb();
	game.getCurrentPlayer()->setSelectedObject(nullptr);
	return handled;
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