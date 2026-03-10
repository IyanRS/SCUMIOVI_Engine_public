#pragma once

#include <functional>
#include <variant>
#include <vector>


#include "Delay.hpp"
#include "EventInfo.hpp"
#include "Vector2.hpp"


const float EVENT_NOT_TRIGGERED_BY_CLICK = -INFINITY;

class Event {
  using Action = std::variant<std::function<void()>,
                              std::function<void(EventInfo)>, Delay>;

  std::vector<Action> actions;

  void executeAfterDelay(Event e, float seconds);

public:
  Event();
  Event(Action action);
  Event(std::vector<Action> actions);
  void execute();
  void execute(EventInfo eventInfo);
  // void addAction(void* action);
  void addAction(Action action);

  Event operator+(const Event &e2);
  Event &operator=(const Event &other);
};