#pragma once

#include <vector>
#include <functional>
#include <variant>

#include "Vector2.hpp"
#include "EventInfo.hpp"

const float EVENT_NOT_TRIGGERED_BY_CLICK = -INFINITY;

class Event {
	using Action = std::variant<
		std::function<void()>,
		std::function<void(EventInfo)>
	>;

	std::vector<Action> actions;

public:
	Event();
	Event(Action action);
	void execute();
	void execute(EventInfo eventInfo);
	//void addAction(void* action);
	void addAction(Action action);

	Event operator+(const Event& e2);
};