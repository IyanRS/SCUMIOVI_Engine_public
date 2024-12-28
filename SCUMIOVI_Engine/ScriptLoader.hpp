#pragma once

#include "Game.hpp"

class ScriptLoader {
	Game& game;
	//Player& player;
	//Scene& currentScene;
	//Camera& currentCamera;

	// Useful events and functions that could be reused in different games
	Event moveToEvent;
	Event pathEvent;

	GameObject& addBackground(Scene& scene, std::string source, Vector2<int> size = Vector2<int>());

public:
	ScriptLoader();
	//Game& loadGame();
	void loadAndRunGame();
};