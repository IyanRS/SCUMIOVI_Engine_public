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
	Verb selectVerb;
	Verb anyVerb; // Represents any possible verb to allow interactions that don't depend on selected verb

	GameObject& addBackground(Scene& scene, std::string source, Vector2<int> size = Vector2<int>());
	GameObject& createUIVerbButton(Verb& verb,
		Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate);
	void createUIVerbButtonMatrix(Vector2<> position, std::vector<std::vector<Verb>> verbs, Texture* verbBackground, Event& onUIUpdate);

public:
	ScriptLoader();
	//Game& loadGame();
	void loadAndRunGame();
};