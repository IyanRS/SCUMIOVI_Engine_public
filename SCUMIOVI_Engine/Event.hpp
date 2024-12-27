#pragma once

#include <list>
#include <functional>
#include <variant>

#include "Vector2.hpp"

const float EVENT_NOT_TRIGGERED_BY_CLICK = -INFINITY;

class Event {
	using Action = std::variant<
		std::function<void()>,
		std::function<void(Vector2<>)>
	>;

	//std::list<void*> actions;
	std::list<Action> actions;

public:
	Event();
	Event(Action action);
	void execute();
	void execute(Vector2<> clickPos);
	//void addAction(void* action);
	void addAction(Action action);
};