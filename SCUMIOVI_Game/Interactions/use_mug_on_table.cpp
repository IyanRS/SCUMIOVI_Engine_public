#include "EngineAPI.hpp"

// Puts the carried mug down on the table. Rather than creating a throwaway
// object, this brings back the mug's own scene Item — hidden, not destroyed,
// since it was picked up — so it keeps its pickup interaction and can simply
// be taken again, as many times as the player likes.
void use_mug_on_table(EventInfo e) {
	auto scene = e.player.getScene();
	if (!scene) return;

	auto mug = scene->getGameObject("mug");
	auto table = dynamic_cast<GameObject*>(&e.interactable);
	if (!mug || !table) return;

	// The inventory copy goes away (the selected object is still set while
	// this interaction runs; interact() clears it afterwards).
	auto carried = e.player.getSelectedObject();
	if (carried) e.player.getInventory().removeItem(carried);

	// Centred on the table, resting on its top edge.
	Vector2<> tablePos = table->getPosition();
	Vector2<> tableSize = table->getSize();
	Vector2<> mugSize = mug->getSize();
	mug->setPosition(Vector2<>(
		tablePos.x + (tableSize.x - mugSize.x) / 2.0f,
		tablePos.y - mugSize.y + 4.0f
	));
	// In front of the table, or it would be hidden behind the tabletop.
	mug->setLayer(table->getLayer() + 1);
	mug->setVisible(true);
	mug->setInteractable(true);

	e.player.say(tr("I'll leave the mug on the table for now."));
	e.game.getUI().updateUI();
}
