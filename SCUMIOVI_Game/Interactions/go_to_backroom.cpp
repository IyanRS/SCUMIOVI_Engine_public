#include "EngineAPI.hpp"

void go_to_backroom(EventInfo e) {
	e.game.changeScene(e.game.findScene("backroom"), 0.5f);
}
