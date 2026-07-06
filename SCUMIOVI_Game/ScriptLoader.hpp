#pragma once

#include "GameDataLoader.hpp"
#include "Event.hpp"
#include "GameObject.hpp"
#include <memory>
#include <string>

class ScriptLoader : public GameDataLoader {
	Game& game;

	Event moveToEvent;
	Event pathEvent;
	// Built in the constructor; wired to game.giveVerb in the generated
	// loadGame() once verbs exist (see GameData.cpp.jinja2).
	Event afterGiveEvent;

	std::shared_ptr<GameObject> createUIVerbButton(std::shared_ptr<Verb> verb,
		Vector2<> position, Vector2<> size, std::shared_ptr<Texture> verbBackground, Event& onUIUpdate);
	std::shared_ptr<GameObject> createUIItemSlot(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> background, Event& onUIUpdate);
	Event highlightDefaultVerbEvent;
	Event dontHighlightDefaultVerbEvent;
	std::shared_ptr<GameObject> addItemObject(std::string name, Scene& scene, std::string sceneTextureSource, std::string inventoryTextureSource, Vector2<> pos);
	Event updateItems;

public:
	ScriptLoader();
	void loadGame(Game& game) override;
};