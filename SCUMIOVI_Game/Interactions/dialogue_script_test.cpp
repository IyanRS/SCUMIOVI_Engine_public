#include "EngineAPI.hpp"

void dialogue_script_test(EventInfo e) {
	e.player.moveTo(Vector2<>(100, 100), 2.0f, Event(std::function<void(EventInfo)>([=](EventInfo e) {
		e.player.say(tr("This is a test of a dialogue script."), 0.0f, Event(std::function<void(EventInfo)>([=](EventInfo e) {
			e.player.say(tr("It can move the player\nand make them say things."), 0.0f, Event(std::function<void(EventInfo)>([=](EventInfo e) {
				e.player.say(tr("And it can chain multiple\nevents together."), 0.0f, Event(std::function<void(EventInfo)>([=](EventInfo e) {
					e.player.say(tr("That's all for now!"), 0.0f);
				})));
			})));
		})));
	})));
}
