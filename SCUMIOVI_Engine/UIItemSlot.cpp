#include "UIItemSlot.hpp"

#include "UI.hpp"
#include "Game.hpp"

UIItemSlot::UIItemSlot(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> slotBackground, int relativeIndex) : GameObject("itemSlot", position, size, slotBackground) {
	setDefaultVerb(Game::getInstance().anyVerb);
	addVerbInteraction(Game::getInstance().anyVerb, Event([relativeIndex](EventInfo e) {
		SDL_Log("Selected item %d", relativeIndex);
		auto item = e.player.getInventory().getItemAt(relativeIndex + e.game.getUI().getInventoryIndex());
		if (item != nullptr)
			item->interact(e.clickPos, e.player.getVerb());
		e.player.setSelectedObject(item);

	}));
	setMouseOverEvent(Event([this, relativeIndex](EventInfo e) {
		e.player.setTargetObject(e.player.getInventory().getItemAt(relativeIndex + e.game.getUI().getInventoryIndex()));
		e.game.getUI().updateUI();
	}));
	setMouseNotOverEvent(Event([this, relativeIndex](EventInfo e) {
		e.player.setTargetObject(nullptr);
		e.game.getUI().updateUI();
	}));
}