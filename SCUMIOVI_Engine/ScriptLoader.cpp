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
	Text text = Text(verb.getDisplayName(), 11, color, false, TEXT_CENTERED);
	uiVerbText->setTextOver(text);
	uiVerbText->setDefaultVerb(&selectVerb);
	std::string verbName = verb.getName();
	uiVerbText->addVerbInteraction(anyVerb, Event([verbName] {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		player.setVerb(verbName);
	}) + onUIUpdate);
	Game& game = Game::getInstance();

	//Event highlightOnMouseHover = Event([](EventInfo e) {
	//	e.game.getUI().getUIElement("uiCurrentVerbInfo")->setTextOver("This is a test");
	//	});

	//uiVerbText->setMouseOverEvent(highlightOnMouseHover);

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
	player.addOverlapEvent(&scummBarBackTrigger, Event([] {
		Game& game = Game::getInstance();
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
	door.setDefaultVerb(&openVerb);
	door.addVerbInteraction(openVerb, Event([] {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		GameObject* door = game.getScene().getGameObject("door");
		door->toggleVisibility();
		if (door->isVisible()) {
			door->addVerbInteraction(*game.getVerb("walk"), Event([] {
				Game& game = Game::getInstance();
				game.getCurrentPlayer().say("Salir");
			}));

			player.say("Abierta", 0, Event([] {
				Game& game = Game::getInstance();
				Player& player = game.getCurrentPlayer();
				player.say("Prueba");
			}));
		}
		else {
			door->removeInteraction("walk");
			player.say("Cerrada");
		}
	}));

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

	player.setVerb("walk");

	walkVerb.setDefaultEvent(&updateUI);

	Texture customCursorTexture = Texture("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1, -1));

	game.bindLeftClickToVerb(nullptr);

	Texture tableTexture2 = Texture("table2", "table2.png");
	GameObject table2 = GameObject("table2", Vector2<>(20, 118), Vector2<>(-1, -1), &tableTexture2);
	table2.setLayer(1);
	scene.addGameObject(table2);

	game.startGame();
}

