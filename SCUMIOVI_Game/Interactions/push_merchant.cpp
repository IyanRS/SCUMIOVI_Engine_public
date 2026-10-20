#include "EngineAPI.hpp"

void push_merchant(EventInfo e) {
	dynamic_cast<GameObject&>(e.interactable).say(tr("Hey, watch it, pal!"));
}
