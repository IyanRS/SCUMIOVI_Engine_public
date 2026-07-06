#include "Door.hpp"
#include "Game.hpp"
#include "Localization.hpp"
#include <memory>

Door::Door(std::string name,
	Vector2<> position,
	std::shared_ptr<Texture> closedTexture, 
	std::shared_ptr<Texture> openTexture,
	std::shared_ptr<Scene> scene) : GameObject::GameObject(name, position, Vector2<>(-1, -1), closedTexture), 
	sceneTransition(name + "Transition", position, getSize(), scene) 
{
	addTexture(closedTexture);
	addTexture(openTexture);

	setTexture(closedTexture->getName());
	setState(CLOSED);

	Game& game = Game::getInstance();
	setDefaultVerb(game.openVerb);

	addVerbInteraction(game.closeVerb, Event([this, closedTexture](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		if (getState() == CLOSED) {
			return;
		}
		auto pos = e.game.getScene()->getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5f;
		pos.y -= e.player.getSize().y * 0.8f;
		e.player.moveToAtSpeed(pos, 100, Event([this, closedTexture](EventInfo e) {
			setTexture(closedTexture->getName());
			e.player.say(tr("Closed"));
			Game& game = Game::getInstance();
			setDefaultVerb(game.openVerb);
			setState(CLOSED);
		}));
	}));

	addVerbInteraction(game.walkVerb, Event([this](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		auto pos = e.game.getScene()->getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5f;
		pos.y -= e.player.getSize().y * 0.8f;
		e.player.moveToAtSpeed(pos, 100, Event([this](EventInfo e) {
			if (getState() == OPEN) {
				e.game.changeScene(sceneTransition.getTargetScene());
			}
		}));
	}));

	addVerbInteraction(game.openVerb, Event([this, openTexture](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		if (getState() == OPEN) {
			return;
		}
		auto pos = e.game.getScene()->getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5f;
		pos.y -= e.player.getSize().y * 0.8f;
		e.player.moveToAtSpeed(pos, 100, Event([this, openTexture](EventInfo e) {
			setTexture(openTexture->getName());
			Game& game = Game::getInstance();
			setDefaultVerb(game.closeVerb);
			setState(OPEN);
			e.player.say(tr("Opened"));
		}));
	}));
}