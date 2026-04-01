#include "Item.hpp"
#include "Game.hpp"

Item::Item(std::string name,
	Vector2<> position,
	Texture* sceneTexture,
	Texture* inventoryTexture) : GameObject::GameObject(name, position, Vector2<>(-1, -1), sceneTexture) {

	setLayer(-1);

	addVerbInteraction(Verb::pickupVerb, Event([this, name, inventoryTexture](EventInfo e) {
		if (e.player.getCurrentHorizontalDirection() == RIGHT)
			e.game.getCurrentPlayer()->playAnimation("interactRight");
		else
			e.game.getCurrentPlayer()->playAnimation("interactLeft");
		
		shared_from_this()->destroy();
		
		auto itemInInventory = std::make_shared<GameObject>(name, Vector2<>(), Vector2<>(-1, -1), inventoryTexture);
		itemInInventory->addTag("item");
		itemInInventory->addVerbInteraction(Verb::giveVerb, Event([](EventInfo e) {
			auto interactableObj = std::dynamic_pointer_cast<GameObject>(static_cast<GameObject&>(e.interactable).shared_from_this());
			if (e.player.getSelectedObject() == nullptr) {
				e.player.setSelectedObject(interactableObj);
				e.player.say("Test1");
			}
			else {
				e.player.setSelectedObject(nullptr);
				e.player.say("Test2");
			}
			}));
		e.player.getInventory().addItem(itemInInventory);
		e.game.getUI().setHighlightedVerb(nullptr);
		e.game.getUI().updateUI();
		}));
		//}) + dontHighlightDefaultVerbEvent);
	setDefaultVerb(&Verb::pickupVerb);
}