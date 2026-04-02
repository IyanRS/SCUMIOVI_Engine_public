#include "SceneTransitionObject.hpp"
#include "Event.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <memory>

SceneTransitionObject::SceneTransitionObject(
	std::string name,
	Vector2<> position,
	Vector2<> size,
	std::shared_ptr<Scene> targetScene
) : GameObject(name, position, size), targetScene(targetScene) {
	Game& game = Game::getInstance();
	addVerbInteraction(game.walkVerb, Event([this](EventInfo e) {
		e.player.say("Test");
	}));
}