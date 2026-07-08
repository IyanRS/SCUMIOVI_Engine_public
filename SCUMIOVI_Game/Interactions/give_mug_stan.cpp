#include "EngineAPI.hpp"

void give_mug_stan(EventInfo e) {
	if (e.player.getCurrentHorizontalDirection() == RIGHT)
		e.player.playAnimation("interactRight");
	else
		e.player.playAnimation("interactLeft");
	e.player.getScene()->getGameObject("stan")->say(tr("Thanks"));
	e.game.getGameState().setFlag("gave_mug_to_stan");
}
