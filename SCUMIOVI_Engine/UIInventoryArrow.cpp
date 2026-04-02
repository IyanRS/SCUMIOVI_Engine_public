#include "UIInventoryArrow.hpp"

#include "Game.hpp"
#include "UILayout.hpp"

UIInventoryArrow::UIInventoryArrow(Vector2<> position, std::shared_ptr<Texture> texture, int increment) : GameObject("inventoryArrow", position, Vector2<>(-1,-1), texture) {
	this->increment = increment;

	addVerbInteraction(Game::getInstance().anyVerb, Event([this](EventInfo e) {
		e.game.getUI().incrementInventoryIndex(this->increment);
		e.game.getUI().updateUI();
	}));
}