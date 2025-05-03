// ==========================================
//  Auto-generated file. Do not edit by hand.
// ==========================================

#include "../../SCUMIOVI_Engine/ScriptLoader.hpp"

void ScriptLoader::loadAndRunGame() {
	// Monkey Island demo

	Game& game = Game::getInstance();

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

    // --- Players ---
    Player guybrush = Player("guybrush", Vector2<>(0,0));
    guybrush.setLayer(0);
    guybrush.addAnimation(&guybrushWalkRight);
    guybrush.addAnimation(&guybrushWalkLeft);
    guybrush.addAnimation(&guybrushIdleRight);
    guybrush.addAnimation(&guybrushIdleLeft);

    Texture idleTexture = guybrushWalkRight.getFrames()[0];
    idleTexture.setName("idle");
    guybrush.addTexture(&idleTexture);
    guybrush.setTexture("idle");

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

    // --- Startup configuration ---
    game.setCurrentPlayer(guybrush);
    game.changeScene(*guybrush.getScene());
    guybrush.moveToScene(scummbar);

    game.startGame();

}