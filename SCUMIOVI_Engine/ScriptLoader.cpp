#include "ScriptLoader.hpp"
#include "Door.hpp"

Event createMoveToEvent(Vector2<> pos, Event callback) {
	return Event([pos, callback](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		std::string textureAfterWalk;
		//if (pos.x < e.player.getPosition().x + e.player.getSize().x / 2) {
		//	e.player.setAnimation("moveLeft");
		//	e.player.setTexture("idleLeft");
		//}
		//else {
		//	e.player.setAnimation("moveRight");
		//	e.player.setTexture("idleRight");
		//}
		e.player.moveToAtSpeed(pos - (Vector2<>(e.player.getSize().x / 2, e.player.getSize().y * 0.85)), 100.0f, Event([] {
			Game& game = Game::getInstance();
			Player& player = game.getCurrentPlayer();
			//player.setAnimation("");
			}) + callback);
		});
}

ScriptLoader::ScriptLoader() : game(Game::getInstance()) {
	moveToEvent = Event([](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		std::string textureAfterWalk;
		//if (e.clickPos.x < e.player.getPosition().x + e.player.getSize().x / 2) {
		//	e.player.setAnimation("walkLeft");
		//	e.player.setTexture("idleLeft");
		//}
		//else {
		//	e.player.setAnimation("walkRight");
		//	e.player.setTexture("idleRight");
		//}
		e.player.moveToAtSpeed(e.clickPos - (Vector2<>(e.player.getSize().x / 2, e.player.getSize().y * 0.85)), 100.0f, Event([] {
			Game& game = Game::getInstance();
			Player& player = game.getCurrentPlayer();
			//player.setAnimation("");
		}));
	});

	pathEvent = Event([](EventInfo e) {
		WalkableArea* walkableArea = e.game.getScene().getWalkableArea();
		Vector2<> playerFeetOffset = Vector2<>(e.player.getSize().x / 2, e.player.getSize().y * 0.85);
		Vector2<> playerFeetPosition = e.player.getPosition() + playerFeetOffset;
		std::vector<Vector2<>> path = walkableArea->findPath(playerFeetPosition, e.clickPos);

		SDL_Log("Path:\n");
		for (Vector2<>& pathPoint : path) {
			SDL_Log("\t%s\n", pathPoint.toString().c_str());
		}

		if (path.size() == 0) {
			return;
		}

		std::vector<Vector2<>> pathAtFeetHeight;
		for (Vector2<> pos : path) {
			pathAtFeetHeight.push_back(pos - playerFeetOffset);
		}

		e.player.moveToAtSpeed(pathAtFeetHeight, 100.0f, {Event()});

		//if (path.size() == 1) {
		//	//createMoveToEvent(path[0], Event()).execute();
		//	return;
		//}

		//Event nextEvent = Event();
		//for (int i = path.size() - 1; i >= 0; i--) {
		//	//nextEvent = createMoveToEvent(path[path.size() - 1], nextEvent);
		//	// TODO: move to function
		//	Vector2<> pos = path[i];
		//	nextEvent = createMoveToEvent(pos, nextEvent);
		//}
		//nextEvent.execute();
	});

	selectVerb = Verb("selectVerb");

	anyVerb = Verb("any");
	selectedVerb = Verb("selected");
	defaultVerb = Verb("default");

	highlightDefaultVerbEvent = Event([this](EventInfo e) {
		std::string verbName = e.interactable.getDefaultVerb()->getName();
		highlightUIVerb(verbName);
	});
	dontHighlightDefaultVerbEvent = Event([](EventInfo e) {
		std::string verbName = e.interactable.getDefaultVerb()->getName();
		auto button = e.game.getUI().getUIElement(verbName + "Button");
		button->setTextOver(button->getTextOver().toString());
	});

	updateItems = Event([](EventInfo e) {


		std::vector< GameObject >::iterator it = e.game.getUI().getUIElements().begin();

		while (it != e.game.getUI().getUIElements().end()) {
			if (it->getName().find("_item") != std::string::npos) {
				it = e.game.getUI().getUIElements().erase(it);
			}
			else ++it;
		}
		//for (GameObject& uiElement : e.game.getUI().getUIElements()) {
		//	if (uiElement.getName().find("item") != std::string::npos) {
		//		
		//	}
		//}

		// TODO: avoid hardcoded values
		int currentIndex = 0;
		int columns = 4;
		int width = 40;
		int height = 22;

		for (GameObject& item : e.player.getInventory().getItems()) {
			int row = currentIndex / 4;
			int column = currentIndex % 4;

			if (row < 2) {
				item.setPosition(Vector2<>(150 + (column * width), 154 + (row * height)));
				e.game.getUI().addUIElement(item);
			}
			else {
				// Arrows logic
			}
			currentIndex++;
		}
		});

	updateUI = Event([](EventInfo e) {
		Verb* currentVerb = e.game.getCurrentPlayer().getVerb();
		if (currentVerb != nullptr) {
			e.game.getUI().getUIElement("uiCurrentVerbInfo")->setTextOver(currentVerb->getDisplayName());
		}
	});
}

GameObject& ScriptLoader::addItemObject(std::string name, Scene& scene, std::string sceneTextureSource, std::string inventoryTextureSource, Vector2<> pos) {
	// TODO: handle pointers memory
	Texture* sceneTexture = new Texture("", sceneTextureSource, Vector2<int>());
	GameObject* itemInScene= new GameObject(name, pos, Vector2<>(-1, -1), sceneTexture);

	itemInScene->setLayer(-1);

	Game& game = Game::getInstance();
	itemInScene->addVerbInteraction(*game.getVerb("pickup"), Event([this, name, inventoryTextureSource](EventInfo e) {
		GameObject& itemInScene = *(GameObject*)&e.interactable;
		itemInScene.destroy();
		Texture* itemInInventoryTexture = new Texture(name + "_item", inventoryTextureSource);
		GameObject* itemInInventory = new GameObject(name + "_item", Vector2<>(), Vector2<>(-1, -1), itemInInventoryTexture);
		e.player.getInventory().addItem(*itemInInventory);
		}) + dontHighlightDefaultVerbEvent + updateItems);
	itemInScene->setDefaultVerb(game.getVerb("pickup"));

	itemInScene->setMouseOverEvent(highlightDefaultVerbEvent);
	itemInScene->setMouseNotOverEvent(dontHighlightDefaultVerbEvent);

	scene.addGameObject(*itemInScene);
	return *itemInScene;
}

//GameObject& ScriptLoader::addDoor() {
//
//}

void ScriptLoader::highlightUIVerb(std::string verbName) {
	for (GameObject& uiElement : game.getUI().getUIElements()) {
		if (uiElement.getName() == verbName + "Button") {
			auto buttonText = uiElement.getTextOver();
			int highlightedColor[4] = { 230, 44, 230, 0xff };
			Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor, buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
			uiElement.setTextOver(highlightedText);
		}
		else {
			uiElement.setTextOver(uiElement.getTextOver().toString());
		}
	}
}

GameObject& ScriptLoader::addBackground(Scene& scene, std::string source, Vector2<int> size) {
	// TODO: handle pointers memory
	Texture* backgroundTexture = new Texture("", source, Vector2<int>(), size);
	GameObject* background = new GameObject("background", Vector2<>(0, 0), Vector2<>(-1, -1), backgroundTexture);
	background->setLayer(-100);

	Game& game = Game::getInstance();
	Verb* walkVerb = game.getVerb("walk");
	if (walkVerb != nullptr) {
		background->setDefaultVerb(walkVerb);
		background->addVerbInteraction(*walkVerb, Event([](EventInfo e) {
			Game& game = Game::getInstance();
			WalkableArea* walkableArea = game.getScene().getWalkableArea();
			if (walkableArea != nullptr) {
				Vector2<> walkTo = walkableArea->getClosestPoint(e.clickPos);
				walkableArea->interact(walkTo, game.getVerb("walk"));
			}
		}));
	}

	scene.addGameObject(*background);
	return *background;
}

// TODO: text format as parameter
GameObject& ScriptLoader::createUIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate) {
	GameObject* uiVerbText = new GameObject(verb.getName() + "Button", position, size, verbBackground);
	int color[4] = { 125, 44, 125, 0xff };
	Text text = Text(verb.getDisplayName(), 11, color, false, TEXT_CENTERED, true);
	uiVerbText->setTextOver(text);
	uiVerbText->setTextFormat(text);
	uiVerbText->setDefaultVerb(&selectVerb);
	std::string verbName = verb.getName();
	uiVerbText->addVerbInteraction(anyVerb, Event([verbName] {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		player.setVerb(verbName);
	}) + onUIUpdate);
	uiVerbText->setMouseOverEvent(Event([this, verbName] {
		highlightUIVerb(verbName);
	}));
	uiVerbText->setMouseNotOverEvent(Event([this, verbName] (EventInfo e) {
		auto button = e.game.getUI().getUIElement(verbName + "Button");
		button->setTextOver(button->getTextOver().toString());
	}));
	Game& game = Game::getInstance();

	game.getUI().addUIElement(*uiVerbText);

	return *uiVerbText;
}

void ScriptLoader::createUIVerbButtonMatrix(Vector2<> position, std::vector<std::vector<Verb>> verbs, Texture* verbBackground, Event& onUIUpdate) {
	int height = 14;
	std::vector<int> columnLengths;

	int columns = verbs[0].size();
	int rows = verbs.size();

	for (int i = 0; i < columns; i++) {
		int maxLengthColumn = 0;
		for (int j = 0; j < rows; j++) {
			if (verbs[j][i].getDisplayName().length() > maxLengthColumn) {
				maxLengthColumn = verbs[j][i].getDisplayName().length();
			}
		}
		columnLengths.push_back(maxLengthColumn);
	}
	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			Vector2<> buttonPos = Vector2<>(position.x, position.y + (height + 2) * j);
			for (int k = 0; k < i; k++) {
				buttonPos.x += columnLengths[k] * 7;
				buttonPos.x += 2;
			}
			createUIVerbButton(verbs[j][i], buttonPos, Vector2<>(columnLengths[i] * 7, height), verbBackground, onUIUpdate);
		}
	}
}

GameObject& ScriptLoader::createUIItemSlot(Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate) {
	GameObject* itemSlot = new GameObject("itemSlot" + position.toString(), position, size, background);

	Game& game = Game::getInstance();
	game.getUI().addUIElement(*itemSlot);

	return *itemSlot;

	//GameObject* uiVerbText = new GameObject(verb.getName() + "Button", position, size, verbBackground);
	//int color[4] = { 125, 44, 125, 0xff };
	//Text text = Text(verb.getDisplayName(), 11, color, false, TEXT_CENTERED, true);
	//uiVerbText->setTextOver(text);
	//uiVerbText->setTextFormat(text);
	//uiVerbText->setDefaultVerb(&selectVerb);
	//std::string verbName = verb.getName();
	//uiVerbText->addVerbInteraction(anyVerb, Event([verbName] {
	//	Game& game = Game::getInstance();
	//	Player& player = game.getCurrentPlayer();
	//	player.setVerb(verbName);
	//	}) + onUIUpdate);
	//uiVerbText->setMouseOverEvent(Event([this, verbName] {
	//	highlightUIVerb(verbName);
	//	}));
	//uiVerbText->setMouseNotOverEvent(Event([this, verbName](EventInfo e) {
	//	auto button = e.game.getUI().getUIElement(verbName + "Button");
	//	button->setTextOver(button->getTextOver().toString());
	//	}));
	//Game& game = Game::getInstance();

	//game.getUI().addUIElement(*uiVerbText);

	//return *uiVerbText;
}

void ScriptLoader::createUIItemSlotsMatrix(Vector2<> position, int columns, int rows, Texture* background, Event& onUIUpdate) {
	int width = 40;
	int height = 22;

	for (int i = 0; i < columns; i++) {
		for (int j = 0; j < rows; j++) {
			Vector2<> slotPos = Vector2<>(position.x + (width + 2) * i, position.y + (height + 2) * j);
			createUIItemSlot(slotPos, Vector2<>(width, height), background, onUIUpdate);
		}
	}
}

//Event ScriptLoader::pathEvent() {
//
//}

void ScriptLoader::loadAndRunGame() {
	// Monkey Island demo

	Scene scene;

	WalkableArea scummBarWalkableArea = WalkableArea(
		{
		Vector2<>(100, 113),
		Vector2<>(116, 125),
		Vector2<>(146, 126),
		Vector2<>(160, 111),
		Vector2<>(216, 126),
		Vector2<>(232, 121),
		Vector2<>(313, 128),
		Vector2<>(332, 111),
		Vector2<>(362, 111),
		Vector2<>(400, 138),
		Vector2<>(501, 125),
		Vector2<>(503, 104),
		Vector2<>(572, 110),
		Vector2<>(626, 142),
		Vector2<>(241, 142),
		Vector2<>(199, 134),
		Vector2<>(161, 142),
		Vector2<>(80, 141),
		Vector2<>(40, 127)
		}
	);

	game.setDefaultVerb(&Verb::walkVerb);
	scummBarWalkableArea.setDefaultVerb(&Verb::walkVerb);
	scummBarWalkableArea.addVerbInteraction(Verb::walkVerb, moveToEvent);
	//scummBarWalkableArea.addVerbInteraction(walkVerb, pathEvent);
	scene.setWalkableArea(&scummBarWalkableArea);
	
	GameObject& background = addBackground(scene, "SCUMM_Bar.png", Vector2<int>(640, 144));

	game.changeScene(scene);



	Animation walkingRightAnimation = Animation("moveRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47), 6);
	Animation walkingLeftAnimation = walkingRightAnimation.getMirrored("moveLeft");

	//Texture idleDown = Texture("idleDown", "Guybrush.png", Vector2<int>(5, 7), Vector2<int>(32, 47));
	Texture idleRightTexture = Texture("idleRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47));
	//Texture idleLeft = idleRight.getMirrored("idleLeft");
	Animation idleRight = Animation("idleRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47), 1);
	Animation idleLeft = idleRight.getMirrored("idleLeft");

	Player player = Player("Guybrush", Vector2<>(80, 80));
	player.setLayer(0);

	//player.addTexture(&idleDown);
	player.addTexture(&idleRightTexture);
	//player.addTexture(&idleLeft);
	player.setTexture("idleRight");

	//player.setMovementAnimations(nullptr, nullptr, &walkingLeftAnimation, &walkingRightAnimation);
	//player.setIdleAnimations(nullptr, nullptr, &idleRight, &idleLeft);

	player.addAnimation(&walkingRightAnimation);
	player.addAnimation(&walkingLeftAnimation);
	player.addAnimation(&idleRight);
	player.addAnimation(&idleLeft);


	player.moveToScene(scene);

	int textColor[] = { 0xff,0xff,0xff,0xff };
	Text guybrushTextFormat = Text("", 12, textColor, true);
	player.setTextFormat(guybrushTextFormat);

	game.addPlayer(player);
	game.setCurrentPlayer(player);

	GameObject scummBarBackTrigger = GameObject("triggerBox", Vector2<>(310, 100), Vector2<>(23, 47));
	scene.addGameObject(scummBarBackTrigger);
	player.addOverlapEvent(&scummBarBackTrigger, Event([] (EventInfo e) {
		Game& game = Game::getInstance();
		game.getCurrentPlayer().disableMovement();
		SDL_Log("Triggerbox event");
		e.player.say("triggerBoxTest");
		if (game.getCurrentPlayer().getDirectionVector().x > 0) {
			game.getCurrentPlayer().moveToAtSpeed(Vector2<>(340, 80), 100.0f, Event([] {
				Game& game = Game::getInstance();
				//game.getCurrentPlayer().setAnimation("");
				//game.getCurrentPlayer().setTexture("idleRight");
				game.getCurrentPlayer().enableMovement();
				}));
			game.getScene().getCamera().moveTo(Vector2<>(320, 0), 2);
		}
		else {
			game.getCurrentPlayer().moveToAtSpeed(Vector2<>(280, 80), 100.0f, Event([] {
				Game& game = Game::getInstance();
				//game.getCurrentPlayer().setAnimation("");
				//game.getCurrentPlayer().setTexture("idleLeft");
				game.getCurrentPlayer().enableMovement();
				}));
			game.getScene().getCamera().moveTo(Vector2<>(0, 0), 2);
		}
		}));

	Texture doorClosedTexture = Texture("doorClosed", "room28_obj_13b_00.png");
	Texture doorOpenTexture = Texture("doorOpen");

	Door door = Door("door", Vector2<>(32, 72), doorClosedTexture, doorOpenTexture, scene);

	
	//GameObject door = GameObject("door", Vector2<>(32, 72), Vector2<>(-1, -1), &doorTexture);
	door.setLayer(-1);
	scene.addGameObject(door);
	Verb openVerb = Verb("open", "Open");

	scummBarWalkableArea.showDebug();

	Texture tableTexture = Texture("table", "table.png");
	GameObject table = GameObject("table", Vector2<>(157, 115), Vector2<>(-1, -1), &tableTexture);
	table.setLayer(1);
	scene.addGameObject(table);

	//game.setStartEvent(startEvent);

	int blackColor[] = { 0x00, 0x00, 0x00, 0xff };
	Texture blackImage = Texture("black", blackColor);
	GameObject uiBackground = GameObject("uiBackground", Vector2<>(0, 144), Vector2<>(640, 56), &blackImage);

	game.getUI().addUIElement(uiBackground);

	/* Monkey island UI:
	Verbs:						Inventory:
	
	Give	Pick up	Use		|	# # #
	Open	Look at	Push	
	Close	Talk to	Pull	|	# # #
	*/

	int verbBackgroundColor[] = { 20, 0, 20, 0xff };
	Texture verbBackground = Texture("white", verbBackgroundColor);

	Verb giveVerb = Verb("give", "Give");
	Verb pickupVerb = Verb("pickup", "Pick up");
	Verb useVerb = Verb("use", "Use");
	//Verb openVerb = Verb("open"); // Already defined before
	Verb lookAtVerb = Verb("lookat", "Look at");
	Verb pushVerb = Verb("push", "Push");
	Verb closeVerb = Verb("close", "Close");
	Verb talkVerb = Verb("talk", "Talk");
	Verb pullVerb = Verb("pull", "Pull");

	

	createUIVerbButtonMatrix(Vector2<>(2, 154), { 

		{giveVerb, pickupVerb, useVerb}, 
		{openVerb, lookAtVerb, pushVerb}, 
		{closeVerb, talkVerb, pullVerb} }, 
		
		&verbBackground, updateUI);

	createUIItemSlotsMatrix(Vector2<>(150, 154), 4, 2, &verbBackground, updateItems);

	GameObject uiCurrentVerbInfo = GameObject("uiCurrentVerbInfo", Vector2<>(160, 148), Vector2<>());
	int color[4] = { 125, 44, 125, 0xff };
	Text currentInfoText = Text("Test info", 11, color, false, TEXT_CENTERED);
	uiCurrentVerbInfo.setTextOver(currentInfoText);
	uiCurrentVerbInfo.setTextFormat(currentInfoText);
	game.getUI().addUIElement(uiCurrentVerbInfo);

	//door.addVerbInteraction(pushVerb, Event([] {
	//	Game& game = Game::getInstance();
	//	game.getCurrentPlayer().say("Pushing door", 1.0f);
	//}));

	//door.setMouseOverEvent(highlightDefaultVerbEvent);
	//door.setMouseNotOverEvent(dontHighlightDefaultVerbEvent);

	//door.setDefaultVerb(&closeVerb);
	//door.addVerbInteraction(closeVerb, Event([this](EventInfo e) {
	//	Game& game = Game::getInstance();
	//	Player& player = game.getCurrentPlayer();
	//	GameObject* door = (GameObject*)&e.interactable;
	//	if (!door->isVisible()) {
	//		return;
	//	}
	//	door->toggleVisibility();
	//	door->removeInteraction("walk");
	//	e.player.say("Cerrada");
	//	door->setDefaultVerb("open");
	//	highlightUIVerb("open");
	//	door->addVerbInteraction(*game.getVerb("open"), Event([this](EventInfo e) {
	//		GameObject* door = (GameObject*)&e.interactable;
	//		if (door->isVisible()) {
	//			return;
	//		}
	//		auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(door->getPosition() + door->getSize() / 2);
	//		door->addVerbInteraction(*e.game.getVerb("walk"), createMoveToEvent(pos, Event([](EventInfo e) {e.player.say("Saliendo..."); })));
	//		door->toggleVisibility();
	//		door->setDefaultVerb("close");
	//		highlightUIVerb("close");
	//		e.player.say("Abierta");
	//	}));
	//}));
	//auto pos = game.getScene().getWalkableArea()->getClosestPoint(door.getPosition() + door.getSize() / 2);
	//door.addVerbInteraction(*game.getVerb("walk"), createMoveToEvent(pos, Event([](EventInfo e) {e.player.say("Saliendo..."); })));

	player.setVerb("walk");

	Verb::walkVerb.setDefaultEvent(&updateUI);

	Texture customCursorTexture = Texture("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1, -1));

	game.bindLeftClickToVerb(&selectedVerb);
	game.bindRightClickToVerb(&defaultVerb);

	Texture tableTexture2 = Texture("table2", "table2.png");
	GameObject table2 = GameObject("table2", Vector2<>(20, 118), Vector2<>(-1, -1), &tableTexture2);
	table2.setLayer(1);
	scene.addGameObject(table2);


	addItemObject("mug", scene, "mug.png", "mug_item.png", Vector2<>(124, 97));
	addItemObject("mug2", scene, "mug.png", "mug_item.png", Vector2<>(106, 95));

	game.startGame();
}

