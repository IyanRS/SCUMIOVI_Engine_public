#include "Item.hpp"
#include "Game.hpp"
#include "Localization.hpp"
#include <memory>

Item::Item(std::string name,
	Vector2<> position,
	std::shared_ptr<Texture> sceneTexture,
	std::shared_ptr<Texture> inventoryTexture) : GameObject::GameObject(name, position, Vector2<>(-1, -1), sceneTexture),
	inventoryTexture(inventoryTexture) {

	setLayer(-1);

	Game& game = Game::getInstance();

	addVerbInteraction(game.pickupVerb, Event([this](EventInfo e) {
		auto currentPlayer = e.game.getCurrentPlayer();
		if (currentPlayer) {
			if (currentPlayer->getCurrentHorizontalDirection() == RIGHT)
				currentPlayer->playAnimation("interactRight");
			else
				currentPlayer->playAnimation("interactLeft");

			// Hidden, not destroyed: the scene object stays registered so a
			// save made before the pickup can restore it (visible and
			// interactable again) on load, and a save made after it can find
			// the Item to rebuild the inventory entry from.
			setVisible(false);
			setInteractable(false);

			currentPlayer->getInventory().addItem(createInventoryObject());
			e.game.getUI().setHighlightedVerb(nullptr);
			e.game.getUI().updateUI();
		}
	}));

	setDefaultVerb(game.pickupVerb);
}

std::shared_ptr<GameObject> Item::createInventoryObject() {
	auto itemInInventory = std::make_shared<GameObject>(getName(), Vector2<>(), Vector2<>(-1, -1), inventoryTexture);
	itemInInventory->addTag("item");
	// Keep the scene object's display name (and its casing rule), so
	// sentences read the same before and after pickup.
	itemInInventory->setDisplayName(getDisplayName(), hasFixedCaseDisplayName());

	// No verb interactions are registered here: selecting an inventory item as
	// the source of a two-object verb ("Give mug to ...", "Use mug on ...") is
	// handled generically for every interactable object by
	// Interactable::interact, for every such verb rather than just "give".

	return itemInInventory;
}
