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

	setDefaultVerb(&Verb::closeVerb);
	addVerbInteraction(Verb::closeVerb, Event([this](EventInfo e) {
		if (getState() == CLOSED) {
			return;
		}
		toggleVisibility();
		removeInteraction("walk");
		e.player.say("Cerrada");
		setDefaultVerb("open");
		setState(CLOSED);
		//highlightUIVerb("open");

	addVerbInteraction(Verb::walkVerb, Event([this](EventInfo e) {
		auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(getPosition() + getSize() / 2);
		pos.x -= e.player.getSize().x * 0.5;
		pos.y -= e.player.getSize().y * 0.8;
		e.player.moveToAtSpeed(pos, 100, Event([this](EventInfo e) {
			if (getState() == OPEN) {
				e.player.say("Saliendo...");
			}
		}));
	}));
	addVerbInteraction(Verb::openVerb, Event([this](EventInfo e) {
		if (getState() == OPEN) {
			return;
		}
		//auto pos = e.game.getScene().getWalkableArea()->getClosestPoint(door->getPosition() + door->getSize() / 2);
		toggleVisibility();
		setDefaultVerb("close");
		setState(OPEN);
		//highlightUIVerb("close");
		e.player.say("Abierta");
		}));
	}));

	//auto pos = game.getScene().getWalkableArea()->getClosestPoint(door.getPosition() + door.getSize() / 2);
	//door.addVerbInteraction(*game.getVerb("walk"), createMoveToEvent(pos, Event([](EventInfo e) {e.player.say("Saliendo..."); })));

}