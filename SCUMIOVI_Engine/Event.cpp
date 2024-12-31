#include "Event.hpp"
#include <SDL3/SDL.h>

#include "Vector2.hpp"

#include "Game.hpp"

Event::Event() {}

Event::Event(Action action) {
	actions.push_back(action);
}

void Event::execute() {
	Game& game = Game::getInstance();
	Player& p = game.getCurrentPlayer();
	Interactable i; // No interactable
	EventInfo eventInfo = EventInfo(Vector2<>(EVENT_NOT_TRIGGERED_BY_CLICK, EVENT_NOT_TRIGGERED_BY_CLICK), p, i);
	execute(eventInfo);
}

void Event::execute(EventInfo eventInfo) {
	for (Action action : actions) {
		// Use std::visit to invoke the correct function depending on the type
		std::visit([eventInfo](auto&& func) {
			// Check if func can accept EventInfo as argument
			if constexpr (std::is_invocable_v<decltype(func), EventInfo>) {
				func(eventInfo);  // Call with EventInfo argument
			}
			// Check if func can be called without arguments
			else if constexpr (std::is_invocable_v<decltype(func)>) {
				func();  // Call with no arguments
			}
		}, action);
	}
}

void Event::addAction(Action action) {
	actions.push_back(action);
}

Event Event::operator+(const Event& e2) {
	Event addedEvents;

	for (Action a : actions) {
		addedEvents.actions.push_back(a);
	}

	for (Action a : e2.actions) {
		addedEvents.actions.push_back(a);
	}

	return addedEvents;
};