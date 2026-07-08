// ==========================================
//  Auto-generated file. Do not edit by hand.
// ==========================================

#include "ScriptLoader.hpp"
#include "EngineAPI.hpp"
#include "Interactions/interactions.h"
#include <memory>


void ScriptLoader::loadGame(Game& game) {

    // --- Verbs ---
    // The engine only provides the "any"/"selected" protocol verbs; every
    // game-facing verb (walk, use, give, ...) comes from game/Verbs.csv.
    game.walkVerb = std::make_shared<Verb>("walk", "Walk to");
    game.addVerb(game.walkVerb);
    game.pushVerb = std::make_shared<Verb>("push", "Push", VerbSyntax({ "Push", "Push $target" }));
    game.addVerb(game.pushVerb);
    game.pushVerb->setDefaultEvent(Event([](EventInfo e) {
        e.player.say(tr("I can't move that."));
    }));
    game.closeVerb = std::make_shared<Verb>("close", "Close", VerbSyntax({ "Close", "Close $target" }));
    game.addVerb(game.closeVerb);
    game.openVerb = std::make_shared<Verb>("open", "Open", VerbSyntax({ "Open", "Open $target" }));
    game.addVerb(game.openVerb);
    game.talkVerb = std::make_shared<Verb>("talk", "Talk to", VerbSyntax({ "Talk to", "Talk to $target" }));
    game.addVerb(game.talkVerb);
    game.giveVerb = std::make_shared<Verb>("give", "Give", VerbSyntax({ "Give", "Give $target", "Give $selected to", "Give $selected to $target" }), true);
    game.addVerb(game.giveVerb);
    game.pickupVerb = std::make_shared<Verb>("pickup", "Pick up", VerbSyntax({ "Pick up", "Pick up $target" }));
    game.addVerb(game.pickupVerb);
    game.useVerb = std::make_shared<Verb>("use", "Use", VerbSyntax({ "Use", "Use $target", "Use $selected on $target" }), true);
    game.addVerb(game.useVerb);
    game.lookAtVerb = std::make_shared<Verb>("lookat", "Look at", VerbSyntax({ "Look at", "Look at $target" }));
    game.addVerb(game.lookAtVerb);
    game.lookAtVerb->setDefaultEvent(Event([](EventInfo e) {
        auto* go = dynamic_cast<GameObject*>(&e.interactable);
        if (go && !go->getExamineText().empty()) {
            e.player.say(tr(go->getExamineText()));
        } else {
            e.player.say(tr("I don't see anything special."));
        }
    }));
    game.pullVerb = std::make_shared<Verb>("pull", "Pull", VerbSyntax({ "Pull", "Pull $target" }));
    game.addVerb(game.pullVerb);
    game.pullVerb->setDefaultEvent(Event([](EventInfo e) {
        e.player.say(tr("I can't move that."));
    }));

    // Engine-provided mechanics (click-to-walk pathfinding, give-removes-item)
    // are wired up here, now that the verbs they attach to exist.
    game.walkVerb->setDefaultEvent(moveToEvent);
    game.giveVerb->setAfterActualEvent(afterGiveEvent);

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
    auto otisIdleRight = std::make_shared<Animation>(
        "otisIdleRight",
        "Stan_transparent.png",
        Vector2<int>(36,9),
        Vector2<int>(37,57),
        1
    );
    auto otisIdleLeft = otisIdleRight->getMirrored("otisIdleLeft");

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
    auto door_to_backroomTexture = std::make_shared<Texture>(
        "door_to_backroom",
        "door.png"
    );
    auto door_to_barTexture = std::make_shared<Texture>(
        "door_to_bar",
        "door.png"
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

    auto guybrushIdleTexture = std::make_shared<Texture>(guybrushWalkRight->getFrames()[0]);
    guybrushIdleTexture->setName("idle");
    guybrush->addTexture(guybrushIdleTexture);
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
    auto otis = std::make_shared<Player>("otis", Vector2<>(0.0f, 0.0f));

    auto otisIdleTexture = std::make_shared<Texture>(otisIdleRight->getFrames()[0]);
    otisIdleTexture->setName("idle");
    otis->addTexture(otisIdleTexture);
    otis->setTexture("idle");

    otis->setMovementAnimations(
        otisIdleRight,
        otisIdleLeft,
        nullptr,
        nullptr,
        otisIdleRight,
        otisIdleLeft,
        nullptr,
        nullptr
    );
    otis->setTextFormat(stan_text_formatFormat);

    game.addPlayer(otis);

    // --- Scenes ---
    auto backroom = std::make_shared<Scene>();
    game.registerScene("backroom", backroom);
    // Walkable area
    auto backroomWalk = std::make_unique<WalkableArea>(std::initializer_list<Vector2<>>{
        Vector2<>(40,140), 
        Vector2<>(40,90), 
        Vector2<>(600,90), 
        Vector2<>(600,140), 
    });
    backroom->setWalkableArea(std::move(backroomWalk));

    // Backgrounds
    auto backroomBg = backroom->addBackground(
        "backroom.png", Vector2<int>(640,144)
    );

    // Objects
    auto door_to_bar = std::make_shared<GameObject>(
        "door_to_bar",
        Vector2<>(60,100),
        Vector2<>(-1.0f, -1.0f),
        door_to_barTexture
    );

    door_to_bar->moveToScene(backroom);
    door_to_bar->setLayer(-1);

    door_to_bar->setExamineText("The door back to the main bar.");

    // Players
    otis->moveToScene(backroom);
    otis->setPosition(Vector2<>(300,100));

    auto scummbar = std::make_shared<Scene>();
    game.registerScene("scummbar", scummbar);
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
    auto door_to_backroom = std::make_shared<GameObject>(
        "door_to_backroom",
        Vector2<>(280,68),
        Vector2<>(-1.0f, -1.0f),
        door_to_backroomTexture
    );

    door_to_backroom->moveToScene(scummbar);
    door_to_backroom->setLayer(-1);

    table->setExamineText("It's a sturdy old table. I wonder how many drinks have been spilled on it.");
    table2->setExamineText("Another table. It matches the first one.");
    mug->setExamineText("A dented tin mug. It's empty.");
    stan->setTextFormat(stan_text_formatFormat);
    stan->setExamineText("A slick-looking salesman with an unnervingly wide smile.");
    door_to_backroom->setExamineText("A heavy wooden door leading further into the bar.");

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
    // Layouts (backgrounds, verb grid, inventory grid, verb-info label, pause
    // menu) are authored as data under assets/ui/*.json and loaded here.
    UILoader::loadLayoutsFromDirectory(game, ResourceManager::getUIDirectory());
    game.getUI().setLayout("inventory");

    // --- Key bindings ---
    // What each key does (e.g. Escape opens the pause menu) is game data too;
    // see assets/keybindings.json and KeyBindings.
    game.getInputManager().getKeyBindings().loadFromFile(ResourceManager::getKeybindingsPath());

    game.getRenderer().setWindowTitle("SCUMIOVI");

    // Custom cursor
    auto customCursorTexture = std::make_shared<Texture>("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1.0f, -1.0f));

	game.setDefaultVerb("walk");

    game.getUI().setDialogueLayout(&game.getUI().getLayout("dialogue"));
    game.getUI().setAfterDialogueLayout(&game.getUI().getLayout("inventory"));

    // ==========================================
    //  Auto-generated dialogue code
    // ==========================================

        // --- DialogueNode for "main" ---

    auto mainNode = std::make_shared<DialogueNode>();
    mainNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            tr("Hello Guybrush"),
            0.6f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                e.game.getCurrentPlayer()->say(
            tr("Hi"),
            0.2f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                        mainNode->showOptions();
                 })));  
             })));  
    })));

        // --- DialogueNode for "label2" ---

    auto label2Node = std::make_shared<DialogueNode>();
    label2Node->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            tr("I'm fine,\nwhat about you?"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                e.game.getCurrentPlayer()->say(
            tr("I'm fine too"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                        label2Node->showOptions();
                 })));  
             })));  
    })));

        // --- DialogueNode for "mugthanks" ---

    auto mugthanksNode = std::make_shared<DialogueNode>();
    mugthanksNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            tr("Best mug I've ever owned"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    mugthanksNode->showOptions();
             })));  
    })));

        // --- DialogueNode for "byelabel" ---

    auto byelabelNode = std::make_shared<DialogueNode>();
    byelabelNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            tr("See you"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    byelabelNode->showOptions();
             })));  
    })));

        // --- DialogueNode for "scripttest" ---

    auto scripttestNode = std::make_shared<DialogueNode>();
    scripttestNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            stan->say(
            tr("Sure, go ahead!"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                        e.game.scriptEvent("dialogue_script_test").execute(e);
             })));  
    })));


    auto mainNodeOption1 = std::make_shared<DialogueOption>(
        "How are you?",
        label2Node
    );
    auto mainNodeOption2 = std::make_shared<DialogueOption>(
        "About that mug...",
        mugthanksNode
    );
    mainNodeOption2->setCondition([]() { return Game::getInstance().getGameState().getFlag("gave_mug_to_stan"); });
    auto mainNodeOption3 = std::make_shared<DialogueOption>(
        "I need to go",
        byelabelNode
    );
    auto label2NodeOption1 = std::make_shared<DialogueOption>(
        "Well, I need to go",
        byelabelNode
    );
    auto label2NodeOption2 = std::make_shared<DialogueOption>(
        "Let's try to execute a script",
        scripttestNode
    );
    auto mugthanksNodeOption1 = std::make_shared<DialogueOption>(
        "See you",
        byelabelNode
    );

    mainNode->setOptions({ mainNodeOption1, mainNodeOption2, mainNodeOption3 });
    label2Node->setOptions({ label2NodeOption1, label2NodeOption2 });
    mugthanksNode->setOptions({ mugthanksNodeOption1 });


    // --- Register named interaction scripts ---
    game.registerScript("give_mug_stan", give_mug_stan);
    game.registerScript("push_stan", push_stan);
    game.registerScript("push_table", push_table);
    game.registerScript("pull_table2", pull_table2);
    game.registerScript("go_to_backroom", go_to_backroom);
    game.registerScript("go_to_bar", go_to_bar);
    game.registerScript("dialogue_script_test", dialogue_script_test);
    game.registerScript("use_mug_on_table", use_mug_on_table);

    stan->addVerbInteraction(game.getVerb("give"), "mug", game.scriptEvent("give_mug_stan"));
    stan->addVerbInteraction(game.getVerb("talk"), Event([=](EventInfo e) {
		e.player.disableMovement();
		e.game.getUI().setLayout(e.game.getUI().getDialogueLayout()->getName());
		mainNode->enterNode();
    }));
    stan->addVerbInteraction(game.getVerb("push"), game.scriptEvent("push_stan"));
    table->addVerbInteraction(game.getVerb("push"), game.scriptEvent("push_table"));
    table2->addVerbInteraction(game.getVerb("pull"), game.scriptEvent("pull_table2"));
    door_to_backroom->addVerbInteraction(game.getVerb("open"), game.scriptEvent("go_to_backroom"));
    door_to_bar->addVerbInteraction(game.getVerb("open"), game.scriptEvent("go_to_bar"));

    // --- Item combinations ---
    game.addCombination("mug", "table", game.scriptEvent("use_mug_on_table"));

    game.bindLeftClickToVerb(game.selectedVerb);
    game.bindRightClickToVerb(game.getDefaultVerb());

    // --- Intro cutscene ---
    // Optional: assets/cutscenes/intro.json is authored data, not engine or
    // game logic; skip silently if the game doesn't ship one.
    if (CutsceneLoader::loadAndRegister(game, ResourceManager::getCutscenesDirectory() + "intro.json", "introCutscene")) {
        game.scriptEvent("introCutscene").execute();
    }

    }