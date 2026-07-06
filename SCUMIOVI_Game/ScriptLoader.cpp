#include "ScriptLoader.hpp"

#include "EngineAPI.hpp"
#include <memory>

ScriptLoader::ScriptLoader() : game(Game::getInstance()) {
	Event walkEvent([](EventInfo e) {
		auto currentPlayer = e.game.getCurrentPlayer();
		if (!currentPlayer) return;

		auto scene = e.game.getScene();
		if (!scene) return;

		WalkableArea* walkableArea = scene->getWalkableArea();
		if (walkableArea != nullptr) {
			Vector2<> walkTo = walkableArea->getClosestPoint(e.clickPos);
			if (!currentPlayer->canMove()) {
				return;
			}
			currentPlayer->moveToAtSpeed(walkTo - (Vector2<>(currentPlayer->getSize().x / 2.0f, currentPlayer->getSize().y * 0.85f)), 100.0f);

			e.game.changeToDefaultVerb();
			currentPlayer->setSelectedObject(nullptr);
			e.game.getUI().updateUI();
		}
	});

	pathEvent = Event([](EventInfo e) {
		auto currentPlayer = e.game.getCurrentPlayer();
		if (!currentPlayer) return;

		auto scene = e.game.getScene();
		if (!scene) return;

		WalkableArea* walkableArea = scene->getWalkableArea();
		if (!walkableArea) return;

		Vector2<> playerFeetOffset = Vector2<>(currentPlayer->getSize().x / 2.0f, currentPlayer->getSize().y * 0.85f);
		Vector2<> playerFeetPosition = currentPlayer->getPosition() + playerFeetOffset;
		std::vector<Vector2<>> path = walkableArea->findPath(playerFeetPosition, e.clickPos);

		if (path.size() == 0) {
			return;
		}

		std::vector<Vector2<>> pathAtFeetHeight;
		for (Vector2<> pos : path) {
			pathAtFeetHeight.push_back(pos - playerFeetOffset);
		}

		currentPlayer->moveToAtSpeed(pathAtFeetHeight, 100.0f, {Event()});
	});

	// walkVerb doesn't exist yet here (it's created from game/Verbs.csv in the
	// generated loadGame()); moveToEvent is wired to it there instead.
	moveToEvent = walkEvent;

	Event highlightDefaultVerbEvent([](EventInfo e) {
		auto defaultVerb = e.interactable.getDefaultVerb();
		if (defaultVerb) {
			e.game.getUI().setHighlightedVerb(defaultVerb);
		}
	});
	
	dontHighlightDefaultVerbEvent = Event([](EventInfo e) {
		auto defaultVerb = e.interactable.getDefaultVerb();
		if (defaultVerb) {
			std::string verbName = defaultVerb->getName();
			auto button = e.game.getUI().getUIElement(verbName + "Button");
			if (button)
				button->setTextOver(button->getTextOver().toString());
		}
	});

	// giveVerb doesn't exist yet here either; wired in the generated loadGame().
	afterGiveEvent = Event([](EventInfo e) {
		auto currentPlayer = e.player.shared_from_this();
		auto selected = e.player.getSelectedObject();
		if (selected != nullptr) {
			e.player.getInventory().removeItem(selected);
			e.game.changeToDefaultVerb();
			e.player.setSelectedObject(nullptr);
			e.game.getUI().updateUI();
		}
	});

	Event noInteractionEvent([](EventInfo e) {
		e.player.say(tr("I can't do that"), 1.0f);
		e.game.changeToDefaultVerb();
		e.player.setSelectedObject(nullptr);
	});
	Verb::setDefaultDefaultEvent(noInteractionEvent);
}

std::shared_ptr<GameObject> ScriptLoader::addItemObject(std::string name, Scene& scene, std::string sceneTextureSource, std::string inventoryTextureSource, Vector2<> pos) {
	std::shared_ptr<Texture> sceneTexture = std::make_shared<Texture>("", sceneTextureSource, Vector2<int>());
	auto itemInScene = std::make_shared<GameObject>(name, pos, Vector2<>(-1.0f, -1.0f), sceneTexture);

	itemInScene->setLayer(-1);

	itemInScene->addVerbInteraction(game.pickupVerb, Event([this, name, inventoryTextureSource](EventInfo e) {
		auto currentPlayer = e.game.getCurrentPlayer();
		if (currentPlayer) {
			if (currentPlayer->getCurrentHorizontalDirection() == RIGHT)
				currentPlayer->playAnimation("interactRight");
			else
				currentPlayer->playAnimation("interactLeft");
			
			auto itemInScenePtr = std::dynamic_pointer_cast<GameObject>(static_cast<GameObject&>(e.interactable).shared_from_this());
			if (itemInScenePtr) {
				itemInScenePtr->destroy();
			}

			std::shared_ptr<Texture> itemInInventoryTexture = std::make_shared<Texture>(name, inventoryTextureSource);
			auto itemInInventory = std::make_shared<GameObject>(name, Vector2<>(), Vector2<>(-1.0f, -1.0f), itemInInventoryTexture);
			itemInInventory->addTag("item");
			
			itemInInventory->addVerbInteraction(game.giveVerb, Event([](EventInfo e) {
				auto interactableObj = std::dynamic_pointer_cast<GameObject>(static_cast<GameObject&>(e.interactable).shared_from_this());
				if (e.player.getSelectedObject() == nullptr) {
					e.player.setSelectedObject(interactableObj);
					e.player.say(tr("Test1"));
				}
				else {
					e.player.setSelectedObject(nullptr);
					e.player.say(tr("Test2"));
				}
			}));
			e.player.getInventory().addItem(itemInInventory);
			e.game.getUI().setHighlightedVerb(nullptr);
			e.game.getUI().updateUI();
		}
	}) + dontHighlightDefaultVerbEvent);
	
	itemInScene->setDefaultVerb(game.pickupVerb);

	scene.addGameObject(itemInScene);
	return itemInScene;
}

std::shared_ptr<GameObject> ScriptLoader::createUIVerbButton(std::shared_ptr<Verb> verb, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> verbBackground, Event& onUIUpdate) {
	auto button = std::make_shared<UIVerbButton>(verb, position, size, verbBackground);
	return button;
}

std::shared_ptr<GameObject> ScriptLoader::createUIItemSlot(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> background, Event& onUIUpdate) {
	auto slot = std::make_shared<GameObject>("ItemSlot", position, size, background);
	return slot;
}