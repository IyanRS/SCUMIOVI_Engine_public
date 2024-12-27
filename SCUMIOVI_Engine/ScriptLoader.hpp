#pragma once

#include "Game.hpp"

class ScriptLoader {
	Game& game;
	//Player& player;
	//Scene& currentScene;
	//Camera& currentCamera;

	// Useful events and scripts that could be reused in different games
	Event moveToEvent;
	Event pathEvent;

public:
	ScriptLoader();
	//Game& loadGame();
	void loadAndRunGame();
};