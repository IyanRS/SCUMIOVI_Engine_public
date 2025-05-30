// ==========================================
//  Auto-generated file. Do not edit by hand.
// ==========================================

#include "../../SCUMIOVI_Engine/ScriptLoader.hpp"
#include "../../SCUMIOVI_Engine/Door.hpp"
#include "../../SCUMIOVI_Engine/Item.hpp"
#include "../../SCUMIOVI_Engine/DialogueNode.hpp"

void ScriptLoader::loadAndRunGame() {
    // --- Animations ---
    Animation guybrushWalkRight = Animation(
        "guybrushWalkRight",
        "Guybrush_walk_side.png",
        Vector2<int>(0,0),
        Vector2<int>(32,47),
        6
    );
    Animation guybrushWalkLeft = guybrushWalkRight.getMirrored("guybrushWalkLeft");
    Animation guybrushIdleRight = Animation(
        "guybrushIdleRight",
        "Guybrush_walk_side.png",
        Vector2<int>(0,0),
        Vector2<int>(32,47),
        1
    );
    Animation guybrushIdleLeft = guybrushIdleRight.getMirrored("guybrushIdleLeft");

    // --- Textures for objects ---
    Texture tableTexture = Texture(
        "table",
        "table.png"
    );
    Texture table2Texture = Texture(
        "table2",
        "table2.png"
    );
    Texture mugTexture = Texture(
        "mug",
        "mug.png"
    );
    Texture mugItemTexture = Texture(
        "mug_item",
        "mug_item.png"
    );
    Texture stanTexture = Texture(
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
    Player guybrush = Player("guybrush", Vector2<>(0,0));

    Texture idleTexture = guybrushWalkRight.getFrames()[0];
    idleTexture.setName("idle");
    guybrush.addTexture(&idleTexture);
    guybrush.setTexture("idle");

    guybrush.setMovementAnimations(
        &guybrushWalkRight,
        &guybrushWalkLeft,
        nullptr,
        nullptr,
        &guybrushIdleRight,
        &guybrushIdleLeft,
        nullptr,
        nullptr
    );
    guybrush.setTextFormat(defaultFormat);

    game.addPlayer(guybrush);

    // --- Scenes ---
    Scene scummbar;
    // Walkable area
    WalkableArea scummbarWalk = WalkableArea({
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
    scummbar.setWalkableArea(&scummbarWalk);

    // Backgrounds
    GameObject& scummbarBg = scummbar.addBackground(
        scummbar, "SCUMM_BAR.png", Vector2<int>(640,144)
    );

    // Objects
    GameObject table(
        "table",
        Vector2<>(157,115),
        Vector2<>(-1,-1),
        &tableTexture
    );

    //scummbar.addGameObject(table);
    table.moveToScene(scummbar);
    table.setLayer(1);
    GameObject table2(
        "table2",
        Vector2<>(20,118),
        Vector2<>(-1,-1),
        &table2Texture
    );

    //scummbar.addGameObject(table2);
    table2.moveToScene(scummbar);
    table2.setLayer(1);
    GameObject mug = Item(
        "mug",
        Vector2<>(124,97),
        &mugTexture,
        &mugItemTexture
    );

    //scummbar.addGameObject(mug);
    mug.moveToScene(scummbar);
    mug.setLayer(-1);
    GameObject stan(
        "stan",
        Vector2<>(155,68),
        Vector2<>(-1,-1),
        &stanTexture
    );

    //scummbar.addGameObject(stan);
    stan.moveToScene(scummbar);
    stan.setLayer(-1);

    stan.setTextFormat(stan_text_formatFormat);

    // Players
    guybrush.moveToScene(scummbar);
    guybrush.setPosition(Vector2<>(80,80));


    // --- Startup configuration ---
    game.setCurrentPlayer(guybrush);
    guybrush.setVerb("walk"); // TODO: custom default verb?
    game.changeScene(*guybrush.getScene());
    guybrush.moveToScene(scummbar);


    // TODO: Custom UI
    int blackColor[] = { 0x00, 0x00, 0x00, 0xff };
	Texture blackImage = Texture("black", blackColor);
	GameObject uiBackground = GameObject("uiBackground", Vector2<>(0, 144), Vector2<>(640, 56), &blackImage);

	game.getUI().createLayout("inventory");
	game.getUI().setLayout("inventory");

	game.getUI().getLayout("inventory").addUIElement(uiBackground);
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

    // Custom cursor
    Texture customCursorTexture = Texture("", "cursor.png");
	game.setCustomCursor(customCursorTexture, Vector2<>(-1, -1));

	game.setDefaultVerb("walk");

    // Interactions
    stan.addVerbInteraction(*game.getVerb("give"), "mug", Event([](EventInfo e) {
		if (e.player.getCurrentHorizontalDirection() == RIGHT)
		    e.player.playAnimation("interactRight");
		else
		    e.player.playAnimation("interactLeft");
		e.player.getScene()->getGameObject("stan")->say("Gracias");
    }));

    game.bindLeftClickToVerb(&Verb::selectedVerb);
    game.bindRightClickToVerb(&Verb::defaultVerb);

    game.startGame();

}