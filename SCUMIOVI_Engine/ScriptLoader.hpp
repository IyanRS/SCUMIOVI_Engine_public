#pragma once

#include "Game.hpp"

class ScriptLoader {
	Game& game;

	// Useful events and functions that could be reused in different games
	Event moveToEvent;
	Event pathEvent;
	Verb selectVerb;

	// TODO: maybe move to Game?
	Verb selectedVerb; // Represents the verb selected by the player
	Verb defaultVerb;  // Represents default verb when interacting with an object
	Verb anyVerb;	   // Represents any possible verb to allow interactions that don't depend on selected verb

	GameObject& addBackground(Scene& scene, std::string source, Vector2<int> size = Vector2<int>());
	GameObject& createUIVerbButton(Verb& verb,
		Vector2<> position, Vector2<> size, Texture* verbBackground, Event& onUIUpdate);
	GameObject& createUIItemSlot(Vector2<> position, Vector2<> size, Texture* background, Event& onUIUpdate);
	void createUIVerbButtonMatrix(Vector2<> position, std::vector<std::vector<Verb>> verbs, Texture* verbBackground, Event& onUIUpdate);
	void createUIItemSlotsMatrix(Vector2<> position, int columns, int rows, Texture* background, Event& onUIUpdate);
	void highlightUIVerb(std::string verbName);
public:
	ScriptLoader();
	//Game& loadGame();
	void loadAndRunGame();
};