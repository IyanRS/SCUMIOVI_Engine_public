#include "ScriptLoader.hpp"
#include "Door.hpp"
#include "DialogueNode.hpp"

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

	highlightDefaultVerbEvent = Event([](EventInfo e) {
		e.game.getUI().setHighlightedVerb(e.interactable.getDefaultVerb());
	});
	dontHighlightDefaultVerbEvent = Event([](EventInfo e) {
		std::string verbName = e.interactable.getDefaultVerb()->getName();
		auto button = e.game.getUI().getUIElement(verbName + "Button");
		if (button)
			button->setTextOver(button->getTextOver().toString());
	});

	//updateUI = Event([](EventInfo e) {
	//	Verb* currentVerb = e.game.getCurrentPlayer().getVerb();
	//	if (currentVerb != nullptr) {
	//		e.game.getUI().getUIElement("uiCurrentVerbInfo")->setTextOver(currentVerb->getDisplayName());
	//	}
	//});
}

GameObject& ScriptLoader::addItemObject(std::string name, Scene& scene, std::string sceneTextureSource, std::string inventoryTextureSource, Vector2<> pos) {
	// TODO: handle pointers memory
	Texture* sceneTexture = new Texture("", sceneTextureSource, Vector2<int>());
	GameObject* itemInScene= new GameObject(name, pos, Vector2<>(-1, -1), sceneTexture);

	itemInScene->setLayer(-1);

	itemInScene->addVerbInteraction(Verb::pickupVerb, Event([this, name, inventoryTextureSource](EventInfo e) {
		GameObject& itemInScene = *(GameObject*)&e.interactable;
		itemInScene.destroy();
		Texture* itemInInventoryTexture = new Texture(name + "_item", inventoryTextureSource);
		GameObject* itemInInventory = new GameObject(name + "_item", Vector2<>(), Vector2<>(-1, -1), itemInInventoryTexture);
		itemInInventory->addVerbInteraction(Verb::giveVerb, Event([](EventInfo e) {
			if (e.player.getSelectedObject() == nullptr) {
				e.player.setSelectedObject((GameObject*) & e.interactable);
				e.player.say("Test1");
			}
			else {
				e.player.setSelectedObject(nullptr);
				e.player.say("Test2");
			}
			}));
		e.player.getInventory().addItem(itemInInventory);
		e.game.getUI().updateUI();
		}) + dontHighlightDefaultVerbEvent);
	itemInScene->setDefaultVerb(&Verb::pickupVerb);

	itemInScene->addVerbInteraction(Verb::pushVerb, Event([](EventInfo e) {
		e.player.say("That wouldn't make any sense");
	}));

	scene.addGameObject(*itemInScene);
	return *itemInScene;
}

void ScriptLoader::loadAndRunGame() {
	// Monkey Island demo

	Event updateUIEvent([this](EventInfo e) {
		game.getUI().updateUI();
	});


	Scene scene;

	WalkableArea scummBarWalkableArea = WalkableArea(
		{
		Vector2<>(100,113),
		Vector2<>(116,125),
		Vector2<>(146,126),
		Vector2<>(160,111),
		Vector2<>(216,126),
		Vector2<>(232,121),
		Vector2<>(313,128),
		Vector2<>(332,111),
		Vector2<>(362,111),
		Vector2<>(400,138),
		Vector2<>(501,125),
		Vector2<>(503,104),
		Vector2<>(572,110),
		Vector2<>(626,142),
		Vector2<>(241,142),
		Vector2<>(199,134),
		Vector2<>(161,142),
		Vector2<>(80,141),
		Vector2<>(40,127),
		}
	);

	game.setDefaultVerb(&Verb::walkVerb);
	scummBarWalkableArea.setDefaultVerb(&Verb::walkVerb);
	scummBarWalkableArea.addVerbInteraction(Verb::walkVerb, moveToEvent);
	//scummBarWalkableArea.addVerbInteraction(walkVerb, pathEvent);
	scene.setWalkableArea(&scummBarWalkableArea);
	
	GameObject& background = scene.addBackground(scene, "SCUMM_Bar.png", Vector2<int>(640, 144));

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

	Texture doorOpenTexture = Texture("doorClosed", "room28_obj_13b_00.png");
	Texture doorClosedTexture = Texture("doorOpen", "", Vector2<int>(), doorOpenTexture.getSize());

	Door door = Door("door", Vector2<>(32, 72), doorClosedTexture, doorOpenTexture, scene);

	
	//GameObject door = GameObject("door", Vector2<>(32, 72), Vector2<>(-1, -1), &doorTexture);
	door.setLayer(-1);
	scene.addGameObject(door);
	//Verb openVerb = Verb("open", "Open");

	scummBarWalkableArea.showDebug();

	Texture tableTexture = Texture("table", "table.png");
	GameObject table = GameObject("table", Vector2<>(157, 115), Vector2<>(-1, -1), &tableTexture);
	table.setLayer(1);
	scene.addGameObject(table);

	//game.setStartEvent(startEvent);

	int blackColor[] = { 0x00, 0x00, 0x00, 0xff };
	Texture blackImage = Texture("black", blackColor);
	GameObject uiBackground = GameObject("uiBackground", Vector2<>(0, 144), Vector2<>(640, 56), &blackImage);

	game.getUI().createLayout("inventory");
	game.getUI().setLayout("inventory");

	game.getUI().getLayout("inventory").addUIElement(uiBackground);

	/* Monkey island UI:
	Verbs:						Inventory:
	
	Give	Pick up	Use		^	# # #
	Open	Look at	Push	
	Close	Talk to	Pull	v	# # #
	*/

	int verbBackgroundColor[] = { 20, 0, 20, 0xff };
	Texture verbBackground = Texture("white", verbBackgroundColor);

	game.getUI().getLayout("inventory").createVerbsMatrix(Vector2<>(2, 154), {

		{&Verb::giveVerb, &Verb::pickupVerb, &Verb::useVerb},
		{&Verb::openVerb, &Verb::lookAtVerb, &Verb::pushVerb},
		{&Verb::closeVerb, &Verb::talkVerb, &Verb::pullVerb} },
		
		&verbBackground);

	Event emptyEvent = Event();

	game.getUI().getLayout("inventory").createInventoryMatrix(Vector2<>(150, 154), 4, 2, &verbBackground, emptyEvent);

	GameObject uiCurrentVerbInfo = GameObject("uiCurrentVerbInfo", Vector2<>(160, 148), Vector2<>());
	int color[4] = { 125, 44, 125, 0xff };
	Text currentInfoText = Text("Test info", 11, color, false, TEXT_CENTERED);
	uiCurrentVerbInfo.setTextOver(currentInfoText);
	uiCurrentVerbInfo.setTextFormat(currentInfoText);
	game.getUI().getLayout("inventory").addUIElement(uiCurrentVerbInfo);

	player.setVerb("walk");

	Verb::walkVerb.setDefaultEvent(&updateUIEvent);

	Texture customCursorTexture = Texture("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1, -1));

	game.bindLeftClickToVerb(&Verb::selectedVerb);
	game.bindRightClickToVerb(&Verb::defaultVerb);

	Texture tableTexture2 = Texture("table2", "table2.png");
	GameObject table2 = GameObject("table2", Vector2<>(20, 118), Vector2<>(-1, -1), &tableTexture2);
	table2.setLayer(1);
	scene.addGameObject(table2);


	addItemObject("mug", scene, "mug.png", "mug_item.png", Vector2<>(124, 97));
	addItemObject("mug2", scene, "mug.png", "mug_item.png", Vector2<>(106, 95));

	Animation stanAnimation = Animation("stanAnimation", "Stan_transparent.png", Vector2<int>(36, 9), Vector2<int>(37, 57), 6, 6);
	GameObject stan = GameObject("stan", Vector2<>(155, 68), Vector2<>(-1,-1), &idleRightTexture);

	stan.setLayer(-1);

	stan.addAnimation(&stanAnimation);
	stan.setAnimation("stanAnimation");
	
	int stanTextColor[] = {0x3e, 0x97, 0xe6, 0xff};
	Text stanTextFormat = Text("", 12, stanTextColor, true);
	stan.setTextFormat(stanTextFormat);

	DialogueOption testOption1 = DialogueOption("Lorem ipsum dolor sit amet, consectetur adipiscing elit.", Event([&stan] (EventInfo e){
		e.player.say("Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit.", 2, Event([&stan](EventInfo e) {
			e.player.say("Cras in imperdiet lorem. Duis aliquam\nvestibulum tellus ac suscipit.", 2, Event([&stan](EventInfo e) {
				e.player.say("Quisque a odio pulvinar, pellentesque\nligula ac, euismod enim.", 2, Event([&stan](EventInfo e) {
					stan.say("Pellentesque quis turpis interdum,\nconsequat nunc a, posuere lacus.", 2, Event([&stan](EventInfo e) {
						e.player.say("Nulla quis viverra felis,\nac vestibulum massa.", 2, Event([&stan](EventInfo e) {
							e.game.getUI().setLayout("inventory");
							e.player.enableMovement();
						}));
					}));
				}));
			}));
		}));
	}));
	DialogueOption testOption2 = DialogueOption("Marcho que tengo que marchar", Event([&stan](EventInfo e) {
		e.player.say("Marcho que tengo que marchar", 1.5, Event([&stan](EventInfo e) {
			stan.say("Te me cuidas", 1.5, Event([](EventInfo e) {
				e.game.getUI().setLayout("inventory");
				e.player.enableMovement();
			}));
		}));
	}));
	DialogueNode testNode = DialogueNode();

	testNode.setOptions({ testOption1, testOption2 });

	//Dialogue testDialogue("dialogue", testNode);

	//stan.setDialogue(&testDialogue);

	game.getUI().createLayout("dialogue");

	game.getUI().getLayout("dialogue").addUIElement(uiBackground);

	stan.addVerbInteraction(Verb::talkVerb, Event([&testNode](EventInfo e) {
		//e.game.getScene().getCamera().setSize(Vector2<>(RENDER_WIDTH * 2, RENDER_HEIGHT * 2));
		e.player.disableMovement();
		e.game.getUI().getLayout("dialogue").createDialogueOptions(testNode);
		e.game.getUI().setLayout("dialogue");
	}));

	stan.setDefaultVerb(&Verb::talkVerb);

	scene.addGameObject(stan);

	game.startGame();

}

