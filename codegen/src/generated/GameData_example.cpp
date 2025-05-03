// ==========================================
//  Auto-generated file. Do not edit by hand.
// ==========================================

#include "../../../SCUMIOVI_Engine/ScriptLoader.hpp"
#include "../../../SCUMIOVI_Engine/Door.hpp"
#include "../../../SCUMIOVI_Engine/DialogueNode.hpp"

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

	//scummBarWalkableArea.setDefaultVerb(&Verb::walkVerb);
	//scummBarWalkableArea.addVerbInteraction(Verb::walkVerb, moveToEvent);
	//scummBarWalkableArea.addVerbInteraction(walkVerb, pathEvent);
	scene.setWalkableArea(&scummBarWalkableArea);
	
	GameObject& background = scene.addBackground(scene, "SCUMM_Bar.png", Vector2<int>(640, 144));

	game.changeScene(scene);



	Animation walkingRightAnimation = Animation("moveRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47), 6);
	Animation walkingLeftAnimation = walkingRightAnimation.getMirrored("moveLeft");
	Animation walkingUpAnimation = Animation("moveUp", "Guybrush.png", Vector2<int>(5, 129), Vector2<int>(30, 48), 6, -4);
	Animation walkingDownAnimation = Animation("moveDown", "Guybrush.png", Vector2<int>(64, 7), Vector2<int>(32, 47), 6, -4);

	//Texture idleDown = Texture("idleDown", "Guybrush.png", Vector2<int>(5, 7), Vector2<int>(32, 47));
	Texture idleRightTexture = Texture("idleRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47));
	//Texture idleLeft = idleRight.getMirrored("idleLeft");

	//Animation idleRight = Animation("idleRight", "Guybrush_walk_side.png", Vector2<int>(0, 0), Vector2<int>(32, 47), 1);
	Animation idleRight = Animation("idleRight", "Guybrush.png", Vector2<int>(17, 178), Vector2<int>(32, 47), 1);
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
	player.addAnimation(&walkingUpAnimation);
	player.addAnimation(&walkingDownAnimation);
	player.addAnimation(&idleRight);
	player.addAnimation(&idleLeft);

	Animation interactRight = Animation("interactRight", "Guybrush.png", Vector2<int>(453, 105), Vector2<int>(32, 47), 1, 0, false, 20);
	Animation interactLeft = interactRight.getMirrored("interactLeft");

	player.addAnimation(&interactRight);
	player.addAnimation(&interactLeft);

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

	Verb::walkVerb.setBeforeActualEvent(&updateUIEvent);

	Texture customCursorTexture = Texture("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1, -1));

	game.bindLeftClickToVerb(&Verb::selectedVerb);
	game.bindRightClickToVerb(&Verb::defaultVerb);

	Texture tableTexture2 = Texture("table2", "table2.png");
	GameObject table2 = GameObject("table2", Vector2<>(20, 118), Vector2<>(-1, -1), &tableTexture2);
	table2.setLayer(1);
	scene.addGameObject(table2);

	auto mug1 = addItemObject("mug", scene, "mug.png", "mug_item.png", Vector2<>(124, 97));
	auto mug2 = addItemObject("mug", scene, "mug.png", "mug_item.png", Vector2<>(106, 95));

	
	//mug2.addVerbInteraction(Verb::giveVerb, Event([](EventInfo e) {e.player.say("Test"); }));

	Animation stanAnimation = Animation("stanAnimation", "Stan_transparent.png", Vector2<int>(36, 9), Vector2<int>(37, 57), 6, 6);
	GameObject stan = GameObject("stan", Vector2<>(155, 68), Vector2<>(-1,-1), &idleRightTexture);

	stan.setLayer(-1);

	stan.addAnimation(&stanAnimation);
	stan.setAnimation("stanAnimation");

	stan.addVerbInteraction(Verb::giveVerb, "mug", Event([&stan](EventInfo e) {
		if (e.player.getCurrentHorizontalDirection() == RIGHT)
			e.player.playAnimation("interactRight");
		else
			e.player.playAnimation("interactLeft");
		stan.say("Gracias");
	}));
	
	int stanTextColor[] = {0x3e, 0x97, 0xe6, 0xff};
	Text stanTextFormat = Text("", 12, stanTextColor, true);
	stan.setTextFormat(stanTextFormat);

	DialogueOption testOption1 = DialogueOption("Lorem ipsum dolor sit amet, consectetur adipiscing elit.", Event([&stan, &testOption1] (EventInfo e){
		e.player.say("Lorem ipsum dolor sit amet,\nconsectetur adipiscing elit.", 1.5, Event([&stan, &testOption1](EventInfo e) {
			e.player.say("Cras in imperdiet lorem. Duis aliquam\nvestibulum tellus ac suscipit.", 1.5, Event([&stan, &testOption1](EventInfo e) {
				e.player.say("Quisque a odio pulvinar, pellentesque\nligula ac, euismod enim.", 1.5, Event([&stan, &testOption1](EventInfo e) {
					stan.say("Pellentesque quis turpis interdum,\nconsequat nunc a, posuere lacus.", 1.5, Event([&stan, &testOption1](EventInfo e) {
						e.player.say("Nulla quis viverra felis,\nac vestibulum massa.", 1.5, Event([&stan, &testOption1](EventInfo e) {
							testOption1.finishDialogue("inventory");
						}));
					}));
				}));
			}));
		}));
	}));
	DialogueOption testOption2 = DialogueOption("Me tengo que ir", Event([&stan, &testOption2](EventInfo e) {
		e.player.say("Me tengo que ir", 1.5, Event([&stan, &testOption2](EventInfo e) {
			stan.say("Nos vemos", 1, Event([&testOption2](EventInfo e) {
				testOption2.finishDialogue("inventory");
			}));
		}));
	}));
	DialogueNode testNode = DialogueNode();

	testOption1.setNextNode(testNode);

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

	//game.getCurrentPlayer().setVerb(&Verb::walkVerb);

	//player.setAnimation("moveUp");

	game.startGame();

}