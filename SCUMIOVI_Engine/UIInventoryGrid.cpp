#include "UIInventoryGrid.hpp"
#include "Game.hpp"
#include <algorithm>

namespace {
	constexpr float kSlotGap = 2.0f;
}

UIInventoryGrid::UIInventoryGrid(std::string name, Vector2<> position, int columns, int rows,
	Vector2<> slotSize, std::shared_ptr<Texture> slotBackground)
	: UIElement(name, position, Vector2<>(0, 0)), columns(columns), slotSize(slotSize) {

	int index = 0;
	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < columns; col++) {
			Vector2<> slotOffset = Vector2<>((slotSize.x + kSlotGap) * col, (slotSize.y + kSlotGap) * row);
			auto slot = std::make_shared<UIItemSlot>(slotOffset, slotSize, slotBackground, index);
			addChild(slot);
			itemSlots.push_back(slot);
			index++;
		}
	}
}

std::vector<std::shared_ptr<UIItemSlot>>& UIInventoryGrid::getItemSlots() {
	return itemSlots;
}

int UIInventoryGrid::getColumns() const {
	return columns;
}

Vector2<> UIInventoryGrid::getSlotSize() const {
	return slotSize;
}

void UIInventoryGrid::refresh() {
	UIElement::refresh();

	// Drop last frame's item icons; slots themselves are never tagged "item".
	auto& kids = getChildren();
	kids.erase(std::remove_if(kids.begin(), kids.end(),
		[](const std::shared_ptr<GameObject>& c) { return c->hasTag("item"); }), kids.end());

	Game& game = Game::getInstance();
	auto player = game.getCurrentPlayer();
	if (!player) return;

	auto& items = player->getInventory().getItems();
	int startIndex = game.getUI().getInventoryIndex();
	int slotCount = (int)itemSlots.size();

	for (int i = 0; i < slotCount; i++) {
		int itemIndex = startIndex + i;
		if (itemIndex >= (int)items.size()) break;

		auto& item = items[itemIndex];
		Vector2<> slotAbsolute = getOffset() + itemSlots[i]->getOffset();
		Vector2<> centerOffset = itemSlots[i]->getSize() / 2 - item->getSize() / 2;
		item->setPosition(slotAbsolute + centerOffset);
		addChild(item);
	}
}
