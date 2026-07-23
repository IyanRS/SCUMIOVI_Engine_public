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
		transition();
	}));
}

std::shared_ptr<Scene> SceneTransitionObject::getTargetScene() {
	return targetScene;
}

void SceneTransitionObject::setEntryPosition(Vector2<> position) {
	entryPosition = position;
}

void SceneTransitionObject::clearEntryPosition() {
	entryPosition.reset();
}

const std::optional<Vector2<>>& SceneTransitionObject::getEntryPosition() const {
	return entryPosition;
}

void SceneTransitionObject::transition(float fadeDuration) const {
	Game::getInstance().changeScene(targetScene, entryPosition, fadeDuration, Event());
}
