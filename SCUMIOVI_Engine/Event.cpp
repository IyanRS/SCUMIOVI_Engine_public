#include "Event.hpp"
#include <SDL3/SDL.h>

#include "Vector2.hpp"

#include "Game.hpp"
#include <thread>

Event::Event() {}

Event::Event(Action action) {
	actions.push_back(action);
}

Event::Event(std::vector<Action> actions) {
	for (Action action : actions) {
		this->actions.push_back(action);
	}
}

void Event::execute() {
	Game& game = Game::getInstance();
	Player& p = game.getCurrentPlayer();
	Interactable i; // No interactable
	EventInfo eventInfo = EventInfo(Vector2<>(EVENT_NOT_TRIGGERED_BY_CLICK, EVENT_NOT_TRIGGERED_BY_CLICK), p, i);
	execute(eventInfo);
}

void Event::executeAfterDelay(Event e, float seconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds((int)(seconds * 1000)));
	e.execute();
}

void Event::execute(EventInfo eventInfo) {
	float delaySeconds = 0;
	int i = 0;
	for (Action action : actions) {
		// Use std::visit to invoke the correct function depending on the type
		std::visit([eventInfo, &delaySeconds](auto&& func) {

			// TODO: maybe create a class for each, might be a better design

			if constexpr (std::is_same_v<std::decay_t<decltype(func)>, Delay>) {
				delaySeconds = func.getSeconds();
			}

			// Check if func can accept EventInfo as argument
			if constexpr (std::is_invocable_v<decltype(func), EventInfo>) {
				func(eventInfo);  // Call with EventInfo argument
			}
			// Check if func can be called without arguments
			else if constexpr (std::is_invocable_v<decltype(func)>) {
				func();  // Call with no arguments
			}
		}, action);

		if (delaySeconds > 0) {
			Event eventAfterDelay = Event();
			for (int j = i + 1; j < actions.size(); j++) {
				eventAfterDelay.addAction(actions[j]);
			}
			auto task = std::bind(&Event::executeAfterDelay, this, eventAfterDelay, delaySeconds);
			std::thread backgroundTask(task);
			backgroundTask.detach();
			break;
		}
		i++;
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

Event& Event::operator=(const Event& other) {
	if (this != &other) {
		for (Action a : other.actions) {
			actions.push_back(a);
		}
	}
	return *this;
}