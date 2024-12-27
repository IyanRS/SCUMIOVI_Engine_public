#include "ScriptLoader.hpp"

ScriptLoader::ScriptLoader() : game(Game::getInstance()) {
	moveToEvent = Event([](Vector2<> clickPos) {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		if (!player.canMove()) {
			return;
		}
		std::string textureAfterWalk;
		if (clickPos.x < player.getPosition().x + player.getSize().x / 2) {
			player.setAnimation("walkLeft");
			player.setTexture("idleLeft");
		}
		else {
			player.setAnimation("walkRight");
			player.setTexture("idleRight");
		}
		player.moveToAtSpeed(clickPos - (Vector2<>(player.getSize().x / 2, player.getSize().y * 0.85)), 100.0f, Event([] {
			Game& game = Game::getInstance();
			Player& player = game.getCurrentPlayer();
			player.setAnimation("");
		}));
	});

	pathEvent = Event([this](Vector2<> clickPos) {
		Player& player = game.getCurrentPlayer();
		WalkableArea* walkableArea = game.getScene().getWalkableArea();
		Vector2<> playerFeetOffset = Vector2<>(player.getSize().x / 2, player.getSize().y * 0.85);
		Vector2<> playerFeetPosition = player.getPosition() + playerFeetOffset;
		std::vector<Vector2<>> path = walkableArea->findPath(playerFeetPosition, clickPos);
		if (path.size() == 0)
			return;
		if (path.size() == 1) {
			player.moveToAtSpeed(path[0] - playerFeetOffset, 100.0f);
			return;
		}

		player.moveToAtSpeed(path, 100.0f);

		//player.moveToAtSpeed(path[0], 100.0f, Event([path] {
		//	Game& game = game.getInstance();
		//	Player& player = game.getCurrentPlayer();
		//	player.moveToAtSpeed(path[1], 100.0f);
		//}));


	});
}

//Event ScriptLoader::pathEvent() {
//
//}

void ScriptLoader::loadAndRunGame() {
	// Monkey Island demo

	Scene scene;

	Texture scummBarBackgroundTexture = Texture("", "SCUMM_Bar.png", Vector2<int>(), Vector2<int>(640, 144));
	GameObject scummBarBackground = GameObject("background", Vector2<>(0, 0), Vector2<>(-1, -1), &scummBarBackgroundTexture);
	scummBarBackground.setLayer(-100);

	//gameObject.getClickEvent().addAction(test);

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

	Verb walkVerb = Verb("walk");
	scummBarWalkableArea.setDefaultVerb(&walkVerb);
	
	//Event walkEvent = createWalkEvent();
	//Event pathEvent = Event([this] (Vector2<> clickPos) {
	//	Game& game = Game::getInstance();
	//	Player& player = game.getCurrentPlayer();
	//	WalkableArea* walkableArea = game.getScene().getWalkableArea();
	//	std::vector<Vector2<>> path = walkableArea->findPath(player.getPosition(), clickPos - (Vector2<>(player.getSize().x / 2, player.getSize().y * 0.85)));

	//	//createWalkEvent();
	//});

	//scummBarWalkableArea.addInteraction(walkVerb, pathEvent);
	scummBarWalkableArea.addVerbInteraction(walkVerb, moveToEvent);
	scene.setWalkableArea(&scummBarWalkableArea);


	game.setDefaultClickEvent(Event([](Vector2<> clickPos) {
		Game& game = Game::getInstance();
		WalkableArea* walkableArea = game.getScene().getWalkableArea();
		if (walkableArea != nullptr) {
			Vector2<> walkTo = walkableArea->getClosestPoint(clickPos);
			walkableArea->interact(walkTo, game.getCurrentPlayer().getVerb("walk"));
		}
	}));


	//scummBarWalkableArea.addInteraction(walkVerb, Event([](Vector2<> clickPos) {
	//	Game& game = Game::getInstance();
	//	WalkableArea* walkableArea = game.getScene().getWalkableArea();
	//	walkableArea->interact(walkableArea->getClosestPoint(clickPos), game.getVerb("walk"));
	//}));


	scene.addGameObject(scummBarBackground);
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
	Verb openVerb = Verb("Open");
	door.setDefaultVerb(&openVerb);
	door.addVerbInteraction(openVerb, Event([] {
		Game& game = Game::getInstance();
		game.getScene().getGameObject("door")->toggleVisibility();
		}));

	scummBarWalkableArea.showDebug();

	Texture tableTexture = Texture("table", "table.png");
	GameObject table = GameObject("table", Vector2<>(157, 115), Vector2<>(-1, -1), &tableTexture);
	table.setLayer(1);
	scene.addGameObject(table);

	//player.setTextOver("This is a test.");

	//game.setStartEvent(startEvent);

	game.startGame();
}

