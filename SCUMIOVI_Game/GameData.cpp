// ==========================================
//  Auto-generated file. Do not edit by hand.
// ==========================================

#include "ScriptLoader.hpp"
#include "EngineAPI.hpp"
#include <memory>

void ScriptLoader::loadGame(Game& game) {

    // --- Animations ---
    auto guybrushWalkRight = std::make_shared<Animation>(
        "guybrushWalkRight",
        "Guybrush_walk_side.png",
        Vector2<int>(0,0),
        Vector2<int>(32,47),
        6
    );
    auto guybrushWalkLeft = guybrushWalkRight->getMirrored("guybrushWalkLeft");
    auto guybrushIdleRight = std::make_shared<Animation>(
        "guybrushIdleRight",
        "Guybrush_walk_side.png",
        Vector2<int>(0,0),
        Vector2<int>(32,47),
        1
    );
    auto guybrushIdleLeft = guybrushIdleRight->getMirrored("guybrushIdleLeft");

    // --- Textures for objects ---
    auto tableTexture = std::make_shared<Texture>(
        "table",
        "table.png"
    );
    auto table2Texture = std::make_shared<Texture>(
        "table2",
        "table2.png"
    );
    auto mugTexture = std::make_shared<Texture>(
        "mug",
        "mug.png"
    );
    auto mugItemTexture = std::make_shared<Texture>(
        "mug_item",
        "mug_item.png"
    );
    auto stanTexture = std::make_shared<Texture>(
        "stan",
        "Stan_transparent.png",
		Vector2<int>(36,9),
		Vector2<int>(37,57)
    );

    // --- Text formats ---
    int defaultFormatColor[] = { 255, 255, 255, 0xff };
    Text defaultFormat = Text(
        "",
        12,
        defaultFormatColor,
        true,
        TEXT_ON_TOP,
        true
    );
    int stan_text_formatFormatColor[] = { 62, 151, 230, 0xff };
    Text stan_text_formatFormat = Text(
        "",
        12,
        stan_text_formatFormatColor,
        true,
        TEXT_ON_TOP,
        true
    );

    // --- Players ---
    auto guybrush = std::make_shared<Player>("guybrush", Vector2<>(0.0f, 0.0f));

    auto idleTexture = std::make_shared<Texture>(guybrushWalkRight->getFrames()[0]);
    idleTexture->setName("idle");
    guybrush->addTexture(idleTexture);
    guybrush->setTexture("idle");

    guybrush->setMovementAnimations(
        guybrushWalkRight,
        guybrushWalkLeft,
        nullptr,
        nullptr,
        guybrushIdleRight,
        guybrushIdleLeft,
        nullptr,
        nullptr
    );
    guybrush->setTextFormat(defaultFormat);

    game.addPlayer(guybrush);

    // --- Scenes ---
    auto scummbar = std::make_shared<Scene>();
    // Walkable area
    auto scummbarWalk = std::make_unique<WalkableArea>(std::initializer_list<Vector2<>>{
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
    });
    scummbar->setWalkableArea(std::move(scummbarWalk));

    // Backgrounds
    auto scummbarBg = scummbar->addBackground(
        "SCUMM_BAR.png", Vector2<int>(640,144)
    );

    // Objects
    auto table = std::make_shared<GameObject>(
        "table",
        Vector2<>(157,115),
        Vector2<>(-1.0f, -1.0f),
        tableTexture
    );

    table->moveToScene(scummbar);
    table->setLayer(1);
    auto table2 = std::make_shared<GameObject>(
        "table2",
        Vector2<>(20,118),
        Vector2<>(-1.0f, -1.0f),
        table2Texture
    );

    table2->moveToScene(scummbar);
    table2->setLayer(1);
    auto mug = std::make_shared<Item>(
        "mug",
        Vector2<>(124,97),
        mugTexture,
        mugItemTexture
    );

    mug->moveToScene(scummbar);
    mug->setLayer(-1);
    auto stan = std::make_shared<GameObject>(
        "stan",
        Vector2<>(155,68),
        Vector2<>(-1.0f, -1.0f),
        stanTexture
    );

    stan->moveToScene(scummbar);
    stan->setLayer(-1);

    stan->setTextFormat(stan_text_formatFormat);

    // Players
    guybrush->moveToScene(scummbar);
    guybrush->setPosition(Vector2<>(80,80));


    // --- Startup configuration ---
    game.setCurrentPlayer(guybrush);
    guybrush->setVerb("walk");
    
    if (game.getCurrentPlayer() && game.getCurrentPlayer()->getScene()) {
        game.changeScene(game.getCurrentPlayer()->getScene());
    }
    
    guybrush->moveToScene(scummbar);

    // --- UI Setup ---
    int blackColor[] = { 0x00, 0x00, 0x00, 0xff };
	auto blackImage = std::make_shared<Texture>("black", blackColor);
	auto uiBackground = std::make_shared<GameObject>("uiBackground", Vector2<>(0.0f, 144.0f), Vector2<>(640.0f, 56.0f), blackImage);

	game.getUI().createLayout("inventory");
	game.getUI().setLayout("inventory");

	game.getUI().getLayout("inventory").addUIElement(uiBackground);
    int verbBackgroundColor[] = { 20, 0, 20, 0xff };
	auto verbBackground = std::make_shared<Texture>("white", verbBackgroundColor);

	game.getUI().getLayout("inventory").createVerbsMatrix(Vector2<>(2.0f, 154.0f), {
		{game.giveVerb, game.pickupVerb, game.useVerb},
		{game.openVerb, game.lookAtVerb, game.pushVerb},
		{game.closeVerb, game.talkVerb, game.pullVerb} },
		verbBackground);

	Event emptyEvent = Event();
	game.getUI().getLayout("inventory").createInventoryMatrix(Vector2<>(150.0f, 154.0f), 4, 2, verbBackground, emptyEvent);

	auto uiCurrentVerbInfo = std::make_shared<GameObject>("uiCurrentVerbInfo", Vector2<>(160.0f, 148.0f), Vector2<>(0.0f, 0.0f));
	int textColorUI[4] = { 125, 44, 125, 0xff };
	Text currentInfoText = Text("Test info", 11, textColorUI, false, TEXT_CENTERED);
	uiCurrentVerbInfo->setTextOver(currentInfoText);
	uiCurrentVerbInfo->setTextFormat(currentInfoText);
	game.getUI().getLayout("inventory").addUIElement(uiCurrentVerbInfo);

    // Custom cursor
    auto customCursorTexture = std::make_shared<Texture>("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1.0f, -1.0f));

	game.setDefaultVerb("walk");

	game.getUI().createLayout("dialogue");
	game.getUI().getLayout("dialogue").addUIElement(uiBackground);
    game.getUI().setDialogueLayout(&game.getUI().getLayout("dialogue"));
    game.getUI().setAfterDialogueLayout(&game.getUI().getLayout("inventory"));

    // ==========================================
    //  Auto-generated dialogue code
    // ==========================================


        // --- DialogueNode for "main" ---

    auto mainNode = std::make_shared<DialogueNode>();
    mainNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            "Hello Guybrush",
            0.6,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                e.game.getCurrentPlayer()->say(
            "Hi",
            0.2,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    mainNode->showOptions();
                 })));  
             })));  
    })));

        // --- DialogueNode for "label2" ---

    auto label2Node = std::make_shared<DialogueNode>();
    label2Node->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            "I'm fine,\nwhat about you?",
            0.0,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                e.game.getCurrentPlayer()->say(
            "I'm fine too",
            0.0,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    label2Node->showOptions();
                 })));  
             })));  
    })));

        // --- DialogueNode for "label3" ---

    auto label3Node = std::make_shared<DialogueNode>();
    label3Node->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            "Are you ok?",
            0.0,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                label3Node->showOptions();
             })));  
    })));

        // --- DialogueNode for "label4" ---

    auto label4Node = std::make_shared<DialogueNode>();
    label4Node->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            "...",
            0.0,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                label4Node->showOptions();
             })));  
    })));

        // --- DialogueNode for "byelabel" ---

    auto byelabelNode = std::make_shared<DialogueNode>();
    byelabelNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            "See you",
            0.0,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                byelabelNode->showOptions();
             })));  
    })));

        // --- DialogueNode for "scripttest" ---

    auto scripttestNode = std::make_shared<DialogueNode>();
    scripttestNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            "What do you want to try?",
            0.0,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    // Action: dialogue_script_test.cpp
             })));  
    })));


    auto mainNodeOption1 = std::make_shared<DialogueOption>(
        "How are you?",
        label2Node
    );
    auto mainNodeOption2 = std::make_shared<DialogueOption>(
        "I need to go",
        byelabelNode
    );
    auto mainNodeOption3 = std::make_shared<DialogueOption>(
        "AAAAa aAaAAaa",
        label3Node
    );
    auto label2NodeOption1 = std::make_shared<DialogueOption>(
        "Well, I need to go",
        byelabelNode
    );
    auto label2NodeOption2 = std::make_shared<DialogueOption>(
        "Let me test something",
        scripttestNode
    );
    auto label3NodeOption1 = std::make_shared<DialogueOption>(
        "AaaaAA aAAAAAAaa AAa",
        label4Node
    );

    mainNode->setOptions({ mainNodeOption1, mainNodeOption2, mainNodeOption3 });
    label2Node->setOptions({ label2NodeOption1, label2NodeOption2 });
    label3Node->setOptions({ label3NodeOption1 });


    stan->addVerbInteraction(game.getVerb("give"), "mug", Event([=](EventInfo e) {
		if (e.player.getCurrentHorizontalDirection() == RIGHT)
		    e.player.playAnimation("interactRight");
		else
		    e.player.playAnimation("interactLeft");
		e.player.getScene()->getGameObject("stan")->say("Gracias");
    }));
    stan->addVerbInteraction(game.getVerb("talk"), Event([=](EventInfo e) {
		e.player.disableMovement();
		e.game.getUI().setLayout(e.game.getUI().getDialogueLayout()->getName());
		mainNode->enterNode();
    }));

    game.bindLeftClickToVerb(game.selectedVerb);
    game.bindRightClickToVerb(game.getDefaultVerb());

    }