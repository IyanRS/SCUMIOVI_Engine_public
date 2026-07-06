#pragma once

#include "UIElement.hpp"
#include "UIItemSlot.hpp"

#include <vector>

// Container for a grid of inventory item slots. Builds its own slots at
// construction (so their geometry is defined exactly once, here) and places
// the player's items into them on refresh using a sliding window starting at
// the active UI's inventory index — replacing the hardcoded column/width/
// height/position values that used to live in UILayout::updateUI().
//
// Note: item icon positions are computed as absolute coordinates (this grid's
// own offset + the target slot's offset), which assumes the grid itself is a
// top-level layout element (not nested under another UIElement's offset).
// That matches how it is used today; nesting it deeper would need the anchor/
// absolute-resolution pass planned for a later phase.
class UIInventoryGrid : public UIElement {
	std::vector<std::shared_ptr<UIItemSlot>> itemSlots;
	int columns;
	Vector2<> slotSize;
public:
	UIInventoryGrid(std::string name, Vector2<> position, int columns, int rows,
		Vector2<> slotSize, std::shared_ptr<Texture> slotBackground);

	std::vector<std::shared_ptr<UIItemSlot>>& getItemSlots();
	int getColumns() const;
	Vector2<> getSlotSize() const;

	void refresh() override;
};
