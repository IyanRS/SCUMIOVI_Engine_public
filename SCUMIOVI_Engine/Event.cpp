#include "Event.hpp"
#include <SDL3/SDL.h>

//Event::Event() {
//	actions = std::list<void*>();
//}

#include "Vector2.hpp"

Event::Event() {}

Event::Event(Action action) {
	actions.push_back(action);
}

void Event::execute() {
	execute(Vector2<>(EVENT_NOT_TRIGGERED_BY_CLICK, EVENT_NOT_TRIGGERED_BY_CLICK));
}

void Event::execute(Vector2<> clickPos) {
	//for (void* action : actions) {
	for (Action action : actions) {



		//for (auto& action : actions) {
		//	// Use std::visit to invoke the correct function depending on the type
		//	std::visit([](auto&& func) {
		//		// Check if the function is invocable with Vector2<int>
		//		if constexpr (std::is_invocable_v<decltype(func), Vector2<int>>) {
		//			func(Vector2<int>(10, 20));  // Call with Vector2<int> argument
		//		}
		//		else if constexpr (std::is_invocable_v<decltype(func)>) {
		//			func();  // Call with no arguments
		//		}
		//		}, action);
		//}

		// Use std::visit to invoke the correct function depending on the type
		std::visit([clickPos](auto&& func) {
			// Check if func can accept Vector2<int> as argument
			//if constexpr (std::is_invocable_v<decltype(func), Vector2<int>>) {
			//	func(Vector2<int>(10, 20));  // Call with Vector2<int> argument
			//}
			// Check if func can accept Vector2<float> as argument
			if constexpr (std::is_invocable_v<decltype(func), Vector2<float>>) {
				func(Vector2<float>(clickPos));  // Call with Vector2<float> argument
			}
			// Check if func can be called without arguments
			else if constexpr (std::is_invocable_v<decltype(func)>) {
				func();  // Call with no arguments
			}
		}, action);


		//action(clickPos);
		//void (*func)() = reinterpret_cast<void(*)()>(action);
		//func();
	}
}

//void Event::addAction(void* action) {
void Event::addAction(Action action) {
	actions.push_back(action);
}