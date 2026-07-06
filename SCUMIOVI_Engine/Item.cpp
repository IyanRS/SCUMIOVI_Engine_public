#include "Item.hpp"
#include "Game.hpp"
#include "Localization.hpp"
#include <memory>

Item::Item(std::string name,
	Vector2<> position,
	std::shared_ptr<Texture> sceneTexture,
	std::shared_ptr<Texture> inventoryTexture) : GameObject::GameObject(name, position, Vector2<>(-1, -1), sceneTexture) {

	setLayer(-1);

	Game& game = Game::getInstance();
	
	addVerbInteraction(game.pickupVerb, Event([this, name, inventoryTexture](EventInfo e) {
		auto currentPlayer = e.game.getCurrentPlayer();
		if (currentPlayer) {
			if (currentPlayer->getCurrentHorizontalDirection() == RIGHT)
				currentPlayer->playAnimation("interactRight");
			else
				currentPlayer->playAnimation("interactLeft");
			
			shared_from_this()->destroy();
			
			auto itemInInventory = std::make_shared<GameObject>(name, Vector2<>(), Vector2<>(-1, -1), inventoryTexture);
			itemInInventory->addTag("item");
			
			Game& internalGame = Game::getInstance();
			itemInInventory->addVerbInteraction(internalGame.giveVerb, Event([](EventInfo e) {
				auto interactableObj = std::dynamic_pointer_cast<GameObject>(static_cast<GameObject&>(e.interactable).shared_from_this());
				auto player = e.game.getCurrentPlayer();
				if (player) {
					if (player->getSelectedObject() == nullptr) {
						player->setSelectedObject(interactableObj);
						player->say(tr("Test1"));
					}
					else {
						player->setSelectedObject(nullptr);
						player->say(tr("Test2"));
					}
				}
			}));
			
			currentPlayer->getInventory().addItem(itemInInventory);
			e.game.getUI().setHighlightedVerb(nullptr);
			e.game.getUI().updateUI();
		}
	}));

	setDefaultVerb(game.pickupVerb);
}