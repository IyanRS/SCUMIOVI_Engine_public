#include "ScriptLoader.hpp"

ScriptLoader::ScriptLoader() : game(Game::getInstance()) {
	moveToEvent = Event([](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		std::string textureAfterWalk;
		if (e.clickPos.x < e.player.getPosition().x + e.player.getSize().x / 2) {
			e.player.setAnimation("walkLeft");
			e.player.setTexture("idleLeft");
		}
		else {
			e.player.setAnimation("walkRight");
			e.player.setTexture("idleRight");
		}
		e.player.moveToAtSpeed(e.clickPos - (Vector2<>(e.player.getSize().x / 2, e.player.getSize().y * 0.85)), 100.0f, Event([] {
			Game& game = Game::getInstance();
			Player& player = game.getCurrentPlayer();
			player.setAnimation("");
		}));
	});

	pathEvent = Event([](EventInfo e) {
		WalkableArea* walkableArea = e.game.getScene().getWalkableArea();
		Vector2<> playerFeetOffset = Vector2<>(e.player.getSize().x / 2, e.player.getSize().y * 0.85);
		Vector2<> playerFeetPosition = e.player.getPosition() + playerFeetOffset;
		std::vector<Vector2<>> path = walkableArea->findPath(playerFeetPosition, e.clickPos);
		if (path.size() == 0)
			return;
		if (path.size() == 1) {
			e.player.moveToAtSpeed(path[0] - playerFeetOffset, 100.0f);
			return;
		}

		e.player.moveToAtSpeed(path, 100.0f);

		//player.moveToAtSpeed(path[0], 100.0f, Event([path] {
		//	Game& game = game.getInstance();
		//	Player& player = game.getCurrentPlayer();
		//	player.moveToAtSpeed(path[1], 100.0f);
		//}));


	});

	selectVerb = Verb("selectVerb");

	anyVerb = Verb("any");
	selectedVerb = Verb("selected");
	defaultVerb = Verb("default");
}

Event createMoveToEvent(static Vector2<> pos, static Event callback) {
	static Vector2<> posCopy = pos;
	static Event callbackCopy = callback;
	return Event([&callback](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		std::string textureAfterWalk;
		if (posCopy.x < e.player.getPosition().x + e.player.getSize().x / 2) {
			e.player.setAnimation("walkLeft");
			e.player.setTexture("idleLeft");
		}
		else {
			e.player.setAnimation("walkRight");
			e.player.setTexture("idleRight");
		}
		e.player.moveToAtSpeed(posCopy - (Vector2<>(e.player.getSize().x / 2, e.player.getSize().y * 0.85)), 100.0f, Event([] {
			Game& game = Game::getInstance();
			Player& player = game.getCurrentPlayer();
			player.setAnimation("");
		}) + callbackCopy);
	});
}

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

	Verb walkVerb = Verb("walk", "Walk to");
	//Event setWalkVerb = Event([] {
	//	Game& game = Game::getInstance();
	//	game.getCurrentPlayer().setVerb("walk");
	//});
	game.setDefaultVerb(&walkVerb);
	scummBarWalkableArea.setDefaultVerb(&walkVerb);
	scummBarWalkableArea.addVerbInteraction(walkVerb, moveToEvent);
	//scummBarWalkableArea.addVerbInteraction(walkVerb, moveToEvent);
	scene.setWalkableArea(&scummBarWalkableArea);
	
	GameObject& background = addBackground(scene, "SCUMM_Bar.png", Vector2<int>(640, 144));

	//Event walkEvent = createWalkEvent();
	//Event pathEvent = Event([this] (Vector2<> clickPos) {
	//	Game& game = Game::getInstance();
	//	Player& player = game.getCurrentPlayer();
	//	WalkableArea* walkableArea = game.getScene().getWalkableArea();
	//	std::vector<Vector2<>> path = walkableArea->findPath(player.getPosition(), clickPos - (Vector2<>(player.getSize().x / 2, player.getSize().y * 0.85)));

	//	//createWalkEvent();
	//});

	//scummBarWalkableArea.addInteraction(walkVerb, pathEvent);


	//game.setDefaultClickEvent(Event([](Vector2<> clickPos) {
	//	Game& game = Game::getInstance();
	//	WalkableArea* walkableArea = game.getScene().getWalkableArea();
	//	if (walkableArea != nullptr) {
	//		Vector2<> walkTo = walkableArea->getClosestPoint(clickPos);
	//		walkableArea->interact(walkTo, game.getVerb("walk"));
	//	}
	//}));


	//scummBarWalkableArea.addInteraction(walkVerb, Event([](Vector2<> clickPos) {
	//	Game& game = Game::getInstance();
	//	WalkableArea* walkableArea = game.getScene().getWalkableArea();
	//	walkableArea->interact(walkableArea->getClosestPoint(clickPos), game.getVerb("walk"));
	//}));


	//scene.getCamera().moveTo(Vector2<>(320, 0), 0, Event());
	game.changeScene(scene);



	Animation walkingRightAnimation = Animation("walkRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47), 6);
	Animation walkingLeftAnimation = walkingRightAnimation.getMirrored("walkLeft");

	Texture idleDown = Texture("idleDown", "Guybrush.png", Vector2<int>(5, 7), Vector2<int>(32, 47));
	Texture idleRight = Texture("idleRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47));
	Texture idleLeft = idleRight.getMirrored("idleLeft");

	Player player = Player("Guybrush", Vector2<>(80, 80));
	player.setLayer(0);

	player.addTexture(&idleDown);
	player.addTexture(&idleRight);
	player.addTexture(&idleLeft);
	player.setTexture("idleRight");

	player.addAnimation(&walkingRightAnimation);
	player.addAnimation(&walkingLeftAnimation);
	player.setTexture("walkLeft");

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
		e.player.say("triggerBoxTest");
		game.getCurrentPlayer().disableMovement();
		if (game.getCurrentPlayer().getDirectionVector().x > 0) {
			game.getCurrentPlayer().moveToAtSpeed(Vector2<>(340, 80), 100.0f, Event([] {
				Game& game = Game::getInstance();
				game.getCurrentPlayer().setAnimation("");
				game.getCurrentPlayer().setTexture("idleRight");
				game.getCurrentPlayer().enableMovement();
				}));
			game.getScene().getCamera().moveTo(Vector2<>(320, 0), 2);
		}
		else {
			game.getCurrentPlayer().moveToAtSpeed(Vector2<>(280, 80), 100.0f, Event([] {
				Game& game = Game::getInstance();
				game.getCurrentPlayer().setAnimation("");
				game.getCurrentPlayer().setTexture("idleLeft");
				game.getCurrentPlayer().enableMovement();
				}));
			game.getScene().getCamera().moveTo(Vector2<>(0, 0), 2);
		}
		}));

	//Event startEvent = Event([] {
	//	Game& game = Game::getInstance();
	//	game.getScene().getCamera().moveTo(Vector2<>(0, 0), 6, Event(
	//		[] { SDL_Log("Callback event test"); }
	//	));
	//	SDL_Log("Start game event test");
	//});

	// path test
	/*Event startEvent = Event([] {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		player.moveToAtSpeed(Vector2<>(0, 0), 100.0f, Event([] {
			Game& game = Game::getInstance();
			Player& player = game.getCurrentPlayer();
			player.moveToAtSpeed(Vector2<>(80, 20), 100.0f, Event([] {
				Game& game = Game::getInstance();
				Player& player = game.getCurrentPlayer();
				player.moveToAtSpeed(Vector2<>(200, 70), 100.0f, Event([] {
					Game& game = Game::getInstance();
					Player& player = game.getCurrentPlayer();
					player.moveToAtSpeed(Vector2<>(100, 0), 100.0f, Event([] {
						Game& game = Game::getInstance();
						Player& player = game.getCurrentPlayer();
						player.moveToAtSpeed(Vector2<>(50, 200), 100.0f, Event([] {
							Game& game = Game::getInstance();
							Player& player = game.getCurrentPlayer();
							player.moveToAtSpeed(Vector2<>(80, 80), 100.0f);
						}));
					}));
				}));
			}));
		}));
	});*/

	Texture doorTexture = Texture("door", "room28_obj_13b_00.png");
	GameObject door = GameObject("door", Vector2<>(32, 72), Vector2<>(-1, -1), &doorTexture);
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

	Event updateItems = Event([](EventInfo e) {


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

			item.setPosition(Vector2<>(150 + (column * width), 154 + (row * height)));
			e.game.getUI().addUIElement(item);
			currentIndex++;
		}
	});

	Event updateUI = Event([](EventInfo e) {
		Verb* currentVerb = e.game.getCurrentPlayer().getVerb();
		if (currentVerb != nullptr) {
			e.game.getUI().getUIElement("uiCurrentVerbInfo")->setTextOver(currentVerb->getDisplayName());
		}
	});

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

	door.addVerbInteraction(pushVerb, Event([] {
		Game& game = Game::getInstance();
		game.getCurrentPlayer().say("Pushing door", 1.0f);
	}));

	Event highlightDefaultVerbEvent = Event([this](EventInfo e) {
		std::string verbName = e.interactable.getDefaultVerb()->getName();
		highlightUIVerb(verbName);
		});
	Event dontHighlightDefaultVerbEvent = Event([](EventInfo e) {
		std::string verbName = e.interactable.getDefaultVerb()->getName();
		auto button = e.game.getUI().getUIElement(verbName + "Button");
		button->setTextOver(button->getTextOver().toString());
	});

	door.setMouseOverEvent(highlightDefaultVerbEvent);
	door.setMouseNotOverEvent(dontHighlightDefaultVerbEvent);


	door.setDefaultVerb(&closeVerb);
	door.addVerbInteraction(closeVerb, Event([&background, this](EventInfo e) {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		GameObject* door = (GameObject*)&e.interactable;
		if (!door->isVisible()) {
			return;
		}
		door->toggleVisibility();
		door->removeInteraction("walk");
		e.player.say("Cerrada");
		door->setDefaultVerb("open");
		highlightUIVerb("open");
		door->addVerbInteraction(*game.getVerb("open"), Event([this](EventInfo e) {
			GameObject* door = (GameObject*)&e.interactable;
			if (door->isVisible()) {
				return;
			}
			auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(door->getPosition() + door->getSize() / 2);
			door->addVerbInteraction(*e.game.getVerb("walk"), createMoveToEvent(pos, Event([](EventInfo e) {e.player.say("Saliendo..."); })));
			door->toggleVisibility();
			door->setDefaultVerb("close");
			highlightUIVerb("close");
			e.player.say("Abierta");
		}));
	}));
	auto pos = game.getScene().getWalkableArea()->getClosestPoint(door.getPosition() + door.getSize() / 2);
	door.addVerbInteraction(*game.getVerb("walk"), createMoveToEvent(pos, Event([](EventInfo e) {e.player.say("Saliendo..."); })));

	player.setVerb("walk");

	walkVerb.setDefaultEvent(&updateUI);

	Texture customCursorTexture = Texture("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1, -1));

	game.bindLeftClickToVerb(&selectedVerb);
	game.bindRightClickToVerb(&defaultVerb);

	Texture tableTexture2 = Texture("table2", "table2.png");
	GameObject table2 = GameObject("table2", Vector2<>(20, 118), Vector2<>(-1, -1), &tableTexture2);
	table2.setLayer(1);
	scene.addGameObject(table2);

	Texture mugTexture = Texture("mug", "mug.png");
	GameObject mug = GameObject("mug", Vector2<>(124, 97), Vector2<>(-1, -1), &mugTexture);
	mug.setLayer(-1);

	mug.addVerbInteraction(pickupVerb, Event([](EventInfo e) {
		GameObject& mug = *(GameObject*)&e.interactable;
		mug.destroy();
		Texture* mugItemTexture = new Texture("mug_item", "mug_item.png");
		GameObject* mugItem = new GameObject("mug_item", Vector2<>(), Vector2<>(-1, -1), mugItemTexture);
		e.player.getInventory().addItem(*mugItem);
		}) + dontHighlightDefaultVerbEvent + updateItems);
	mug.setDefaultVerb(&pickupVerb);

	mug.setMouseOverEvent(highlightDefaultVerbEvent);
	mug.setMouseNotOverEvent(dontHighlightDefaultVerbEvent);

	scene.addGameObject(mug);

	game.startGame();
}

