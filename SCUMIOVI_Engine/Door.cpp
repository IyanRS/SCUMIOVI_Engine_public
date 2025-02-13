#include "Door.hpp"
#include "Game.hpp"

Door::Door(std::string name,
	Vector2<> position,
	Texture& closedTexture, 
	Texture& openTexture,
	Scene& scene) : GameObject::GameObject(name, position, Vector2<>(-1, -1), &closedTexture), 
	sceneTransition(name + "Transition", position, getSize(), scene) {

	//setMouseOverEvent(highlightDefaultVerbEvent);
	//setMouseNotOverEvent(dontHighlightDefaultVerbEvent);
	addTexture(&closedTexture);
	addTexture(&openTexture);

	setTexture(closedTexture.getName());
	setState(CLOSED);

	setDefaultVerb(&Verb::openVerb);
	addVerbInteraction(Verb::closeVerb, Event([this, &closedTexture](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		if (getState() == CLOSED) {
			return;
		}
		auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5;
		pos.y -= e.player.getSize().y * 0.8;
		e.player.moveToAtSpeed(pos, 100, Event([this, &closedTexture](EventInfo e) {
			setTexture(closedTexture.getName());
			e.player.say("Cerrada");
			setDefaultVerb(&Verb::openVerb);
			setState(CLOSED);
		}));
	}));

	addVerbInteraction(Verb::walkVerb, Event([this](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5;
		pos.y -= e.player.getSize().y * 0.8;
		e.player.moveToAtSpeed(pos, 100, Event([this](EventInfo e) {
			if (getState() == OPEN) {
				e.player.say("Saliendo...");
			}
		}));
	}));
	addVerbInteraction(Verb::openVerb, Event([this, &openTexture](EventInfo e) {
		if (!e.player.canMove()) {
			return;
		}
		if (getState() == OPEN) {
			return;
		}
		auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5;
		pos.y -= e.player.getSize().y * 0.8;
		e.player.moveToAtSpeed(pos, 100, Event([this, &openTexture](EventInfo e) {
			setTexture(openTexture.getName());
			setDefaultVerb(&Verb::closeVerb);
			setState(OPEN);
			e.player.say("Abierta");
		}));
		
	}));
}