#include "Event.hpp"
#include <SDL3/SDL.h>

#include "Vector2.hpp"
#include "Game.hpp"

Event::Event() {}

Event::Event(Action action) {
	actions.push_back(action);
}

Event::Event(std::vector<Action> actions) {
	for (Action action : actions) {
		this->actions.push_back(action);
	}
}

// -------------------------------------------------------------------------
// Pending-event queue (static, process-lifetime storage)
// -------------------------------------------------------------------------
struct PendingEvent {
	Event event;
	EventInfo* info;
	float remainingSec;
};

static std::vector<PendingEvent>& pendingQueue() {
	static std::vector<PendingEvent> queue;
	return queue;
}

// -------------------------------------------------------------------------
// execute() — synchronous entry point (no EventInfo from a click)
// -------------------------------------------------------------------------
void Event::execute() {
	Game& game = Game::getInstance();
	Player& p = game.getCurrentPlayer();
	Interactable i; // No interactable
	EventInfo eventInfo = EventInfo(Vector2<>(EVENT_NOT_TRIGGERED_BY_CLICK, EVENT_NOT_TRIGGERED_BY_CLICK), p, i);
	execute(eventInfo);
}

// -------------------------------------------------------------------------
// execute(EventInfo) — runs the action list, stops at the first Delay and
// schedules the remainder in the pending queue instead of sleeping a thread.
// -------------------------------------------------------------------------
void Event::execute(EventInfo eventInfo) {
	float delaySeconds = 0;
	int i = 0;
	for (Action action : actions) {
		std::visit([eventInfo, &delaySeconds](auto&& func) {

			if constexpr (std::is_same_v<std::decay_t<decltype(func)>, Delay>) {
				delaySeconds = func.getSeconds();
			}

			// Check if func can accept EventInfo as argument
			if constexpr (std::is_invocable_v<decltype(func), EventInfo>) {
				func(eventInfo);
			}
			// Check if func can be called without arguments
			else if constexpr (std::is_invocable_v<decltype(func)>) {
				func();
			}
		}, action);

		if (delaySeconds > 0) {
			// Build an event containing every action that comes after the delay
			Event eventAfterDelay = Event();
			for (int j = i + 1; j < (int)actions.size(); j++) {
				eventAfterDelay.addAction(actions[j]);
			}
			// Push into the per-frame queue instead of spinning up a thread
			PendingEvent pending;
			pending.event = eventAfterDelay;
			pending.info  = nullptr; // will re-derive EventInfo on fire
			pending.remainingSec = delaySeconds;
			pendingQueue().push_back(pending);
			break;
		}
		i++;
	}
}

// -------------------------------------------------------------------------
// tick(dt) — called once per frame from the game loop.
// Counts down pending events and fires any that have expired.
// -------------------------------------------------------------------------
void Event::tick(float deltaTime) {
	auto& queue = pendingQueue();

	// Iterate manually so we can safely erase while looping
	for (int i = (int)queue.size() - 1; i >= 0; i--) {
		queue[i].remainingSec -= deltaTime;
		if (queue[i].remainingSec <= 0.0f) {
			// Fire — reconstruct a default EventInfo if none was stored
			queue[i].event.execute();
			queue.erase(queue.begin() + i);
		}
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
		actions.clear();
		for (Action a : other.actions) {
			actions.push_back(a);
		}
	}
	return *this;
}