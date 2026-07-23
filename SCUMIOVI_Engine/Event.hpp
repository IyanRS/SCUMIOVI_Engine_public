#pragma once

#include <functional>
#include <variant>
#include <vector>


#include "Delay.hpp"
#include "EventInfo.hpp"
#include "Vector2.hpp"


const float EVENT_NOT_TRIGGERED_BY_CLICK = -INFINITY;

class Event {
public:
  using Action = std::variant<std::function<void()>,
                              std::function<void(EventInfo)>, Delay>;
private:
  std::vector<Action> actions;



public:
  Event();
  Event(Action action);
  Event(std::vector<Action> actions);
  void execute();
  void execute(EventInfo eventInfo);
  void addAction(Action action);
  // True if executing this event actually does something.
  bool hasActions() const;

  // Called once per frame by the game loop. Advances delay timers
  // and fires events whose timers have expired.
  static void tick(float deltaTime);

  Event operator+(const Event &e2);
  Event &operator=(const Event &other);
};