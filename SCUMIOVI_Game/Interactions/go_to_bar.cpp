#include "EngineAPI.hpp"

void go_to_bar(EventInfo e) {
	e.game.changeScene(e.game.findScene("scummbar"), 0.5f);
}
