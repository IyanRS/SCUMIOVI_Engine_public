#include "EngineAPI.hpp"

void use_mug_on_table(EventInfo e) {
	e.player.say(tr("I'll leave the mug on the table for now."));
}
