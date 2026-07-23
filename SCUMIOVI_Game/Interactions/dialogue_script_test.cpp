#include "EngineAPI.hpp"

// Built as a Cutscene rather than hand-nested callbacks. Each step's
// completion starts the next one (the same chaining, generated for us), and
// the sequence holds the player's input for its whole duration — which is
// what makes it reliable: a stray click used to call moveTo/moveToAtSpeed on
// the player, and that REPLACES the pending movement callback (see
// MovementComponent::moveTo), silently dropping the rest of the chain.
void dialogue_script_test(EventInfo e) {
	auto player = e.game.getCurrentPlayer();
	if (!player) return;

	Cutscene()
		.disableMovementFor(player)
		.walk(player, Vector2<>(70, 80), 0.5f)
		.say(player, "This is a test of a dialogue script.")
		.walk(player, Vector2<>(150, 100), 0.5f)
		.say(player, "It can move the player\nand make them say things.")
		.walk(player, Vector2<>(50, 100), 0.5f)
		.say(player, "And it can chain multiple\nevents together.")
		.walk(player, Vector2<>(150, 80), 0.5f)
		.say(player, "That's all for now!")
		.build()
		.execute();
}
