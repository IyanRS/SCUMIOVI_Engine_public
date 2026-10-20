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
    game.useVerb = std::make_shared<Verb>("use", "Use", VerbSyntax({ "Use", "Use $target", "Use $selected on", "Use $selected on $target" }), true);
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
    auto heroWalkRight = std::make_shared<Animation>(
        "heroWalkRight",
        "hero_walk.png",
        Vector2<int>(0,0),
        Vector2<int>(32,47),
        6
    );
    auto heroWalkLeft = heroWalkRight->getMirrored("heroWalkLeft");
    auto heroIdleRight = std::make_shared<Animation>(
        "heroIdleRight",
        "hero_walk.png",
        Vector2<int>(0,0),
        Vector2<int>(32,47),
        1
    );
    auto heroIdleLeft = heroIdleRight->getMirrored("heroIdleLeft");
    auto deckhandIdleLeft = std::make_shared<Animation>(
        "deckhandIdleLeft",
        "deckhand.png",
        Vector2<int>(0,0),
        Vector2<int>(32,58),
        1
    );
    auto deckhandIdleRight = deckhandIdleLeft->getMirrored("deckhandIdleRight");

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
    auto merchantTexture = std::make_shared<Texture>(
        "merchant",
        "merchant.png"
    );
    auto door_to_backroomOpenTexture = std::make_shared<Texture>(
        "door_to_backroom_open",
        "door.png"
    );
    auto door_to_barOpenTexture = std::make_shared<Texture>(
        "door_to_bar_open",
        "bar_door.png"
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
    int merchant_text_formatFormatColor[] = { 62, 151, 230, 0xff };
    Text merchant_text_formatFormat = Text(
        "",
        12,
        merchant_text_formatFormatColor,
        true,
        TEXT_ON_TOP,
        true
    );

    // --- Players ---
    auto hero = std::make_shared<Player>("hero", Vector2<>(0.0f, 0.0f));

    auto heroIdleTexture = std::make_shared<Texture>(heroWalkRight->getFrames()[0]);
    heroIdleTexture->setName("idle");
    hero->addTexture(heroIdleTexture);
    hero->setTexture("idle");

    hero->setMovementAnimations(
        heroWalkRight,
        heroWalkLeft,
        nullptr,
        nullptr,
        heroIdleRight,
        heroIdleLeft,
        nullptr,
        nullptr
    );
    hero->setTextFormat(defaultFormat);

    game.addPlayer(hero);
    auto deckhand = std::make_shared<Player>("deckhand", Vector2<>(0.0f, 0.0f));

    auto deckhandIdleTexture = std::make_shared<Texture>(deckhandIdleRight->getFrames()[0]);
    deckhandIdleTexture->setName("idle");
    deckhand->addTexture(deckhandIdleTexture);
    deckhand->setTexture("idle");

    deckhand->setMovementAnimations(
        deckhandIdleRight,
        deckhandIdleLeft,
        nullptr,
        nullptr,
        deckhandIdleRight,
        deckhandIdleLeft,
        nullptr,
        nullptr
    );
    deckhand->setTextFormat(merchant_text_formatFormat);

    game.addPlayer(deckhand);

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
    auto backroom_door_to_bar = std::make_shared<Door>(
        "door_to_bar",
        Vector2<>(60,40),
        nullptr,
        door_to_barOpenTexture,
        game.findScene("tavern")
    );
    backroom_door_to_bar->setDisplayName("Bar Door");
    backroom_door_to_bar->setEntryPosition(Vector2<>(560,80));

    backroom_door_to_bar->moveToScene(backroom);
    backroom_door_to_bar->setLayer(-1);
    auto& door_to_bar = backroom_door_to_bar;

    backroom_door_to_bar->setExamineText("The door back to the main bar.");

    // Players
    deckhand->moveToScene(backroom);
    deckhand->setPosition(Vector2<>(300,78));

    auto tavern = std::make_shared<Scene>();
    game.registerScene("tavern", tavern);
    // Walkable area
    auto tavernWalk = std::make_unique<WalkableArea>(std::initializer_list<Vector2<>>{
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
    tavern->setWalkableArea(std::move(tavernWalk));

    tavern->getCamera().setZones({
        { 0.0f, 320.0f, Vector2<>(0.0f, 0.0f) },
        { 320.0f, 640.0f, Vector2<>(320.0f, 0.0f) },
    });
    tavern->getCamera().setObjectToFollow(hero);

    // Backgrounds
    auto tavernBg = tavern->addBackground(
        "tavern.png", Vector2<int>(640,144)
    );

    // Objects
    auto tavern_table = std::make_shared<GameObject>(
        "table",
        Vector2<>(157,115),
        Vector2<>(-1.0f, -1.0f),
        tableTexture
    );
    tavern_table->setDisplayName("Table");

    tavern_table->moveToScene(tavern);
    tavern_table->setLayer(1);
    auto& table = tavern_table;
    auto tavern_table2 = std::make_shared<GameObject>(
        "table2",
        Vector2<>(20,118),
        Vector2<>(-1.0f, -1.0f),
        table2Texture
    );
    tavern_table2->setDisplayName("Wooden Table");

    tavern_table2->moveToScene(tavern);
    tavern_table2->setLayer(1);
    auto& table2 = tavern_table2;
    auto tavern_mug = std::make_shared<Item>(
        "mug",
        Vector2<>(124,97),
        mugTexture,
        mugItemTexture
    );
    tavern_mug->setDisplayName("Mug");

    tavern_mug->moveToScene(tavern);
    tavern_mug->setLayer(-1);
    auto& mug = tavern_mug;
    auto tavern_merchant = std::make_shared<GameObject>(
        "merchant",
        Vector2<>(155,68),
        Vector2<>(-1.0f, -1.0f),
        merchantTexture
    );
    tavern_merchant->setDisplayName("=Vince");

    tavern_merchant->moveToScene(tavern);
    tavern_merchant->setLayer(-1);
    auto& merchant = tavern_merchant;
    auto tavern_door_to_backroom = std::make_shared<Door>(
        "door_to_backroom",
        Vector2<>(594,42),
        nullptr,
        door_to_backroomOpenTexture,
        game.findScene("backroom")
    );
    tavern_door_to_backroom->setDisplayName("Backroom Door");
    tavern_door_to_backroom->setEntryPosition(Vector2<>(90,85));

    tavern_door_to_backroom->moveToScene(tavern);
    tavern_door_to_backroom->setLayer(-1);
    auto& door_to_backroom = tavern_door_to_backroom;
    auto tavern_door_to_bar = std::make_shared<Door>(
        "door_to_bar",
        Vector2<>(32,52),
        nullptr,
        door_to_barOpenTexture,
        game.findScene("tavern")
    );
    tavern_door_to_bar->setDisplayName("Bar Door");
    tavern_door_to_bar->setEntryPosition(Vector2<>(560,80));

    tavern_door_to_bar->moveToScene(tavern);
    tavern_door_to_bar->setLayer(-1);

    tavern_table->setExamineText("It's a sturdy old table. I wonder how many drinks have been spilled on it.");
    tavern_table2->setExamineText("Another table. It matches the first one.");
    tavern_mug->setExamineText("A dented tin mug. It's empty.");
    tavern_merchant->setTextFormat(merchant_text_formatFormat);
    tavern_merchant->setExamineText("A slick-looking salesman with\nan unnervingly wide smile.");
    tavern_door_to_backroom->setExamineText("A heavy wooden door leading further into the bar.");
    tavern_door_to_bar->setExamineText("The door back to the main bar.");

    // Players
    hero->moveToScene(tavern);
    hero->setPosition(Vector2<>(80,80));


    // --- Startup configuration ---
    game.setCurrentPlayer(hero);
    hero->setVerb("walk");
    
    if (game.getCurrentPlayer() && game.getCurrentPlayer()->getScene()) {
        game.changeScene(game.getCurrentPlayer()->getScene());
    }
    
    hero->moveToScene(tavern);

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
            merchant->say(
            tr("Hello, stranger"),
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
            merchant->say(
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
            merchant->say(
            tr("Best mug I've ever owned"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    mugthanksNode->showOptions();
             })));  
    })));

        // --- DialogueNode for "byelabel" ---

    auto byelabelNode = std::make_shared<DialogueNode>();
    byelabelNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            merchant->say(
            tr("See you"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                    byelabelNode->showOptions();
             })));  
    })));

        // --- DialogueNode for "scripttest" ---

    auto scripttestNode = std::make_shared<DialogueNode>();
    scripttestNode->setDialogueEvent(Event(std::function<void(EventInfo)>([=](EventInfo e) {
            merchant->say(
            tr("Sure, go ahead!"),
            0.0f,
            Event(std::function<void(EventInfo)>([=](EventInfo e) {
                        e.game.scriptEvent("dialogue_script_test").execute(e);
                        scripttestNode->showOptions();
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
    mainNodeOption2->setCondition([]() { return Game::getInstance().getGameState().getFlag("gave_mug_to_merchant"); });
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
    game.registerScript("give_mug_merchant", give_mug_merchant);
    game.registerScript("push_merchant", push_merchant);
    game.registerScript("push_table", push_table);
    game.registerScript("use_mug_on_table", use_mug_on_table);
    game.registerScript("pull_table2", pull_table2);
    game.registerScript("dialogue_script_test", dialogue_script_test);
    game.registerScript("show_debug", show_debug);

    merchant->addVerbInteraction(game.getVerb("give"), "mug", game.scriptEvent("give_mug_merchant"));
    merchant->addVerbInteraction(game.getVerb("talk"), Event([=](EventInfo e) {
		e.player.disableMovement();
		e.game.getUI().setLayout(e.game.getUI().getDialogueLayout()->getName());
		mainNode->enterNode();
    }));
    merchant->addVerbInteraction(game.getVerb("push"), game.scriptEvent("push_merchant"));
    table->addVerbInteraction(game.getVerb("push"), game.scriptEvent("push_table"));
    table->addVerbInteraction(game.getVerb("use"), "mug", game.scriptEvent("use_mug_on_table"));
    table2->addVerbInteraction(game.getVerb("pull"), game.scriptEvent("pull_table2"));

    game.bindLeftClickToVerb(game.selectedVerb);
    game.bindRightClickToVerb(game.getDefaultVerb());

    // --- Intro cutscene ---
    // Optional: assets/cutscenes/intro.json is authored data, not engine or
    // game logic; skip silently if the game doesn't ship one.
    if (CutsceneLoader::loadAndRegister(game, ResourceManager::getCutscenesDirectory() + "intro.json", "introCutscene")) {
        game.scriptEvent("introCutscene").execute();
    }

    }