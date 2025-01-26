#include "SceneTransitionObject.hpp"
#include "Event.hpp"
#include "Player.hpp"

SceneTransitionObject::SceneTransitionObject(
	std::string name,
	Vector2<> position,
	Vector2<> size,
	Scene& scene
) : GameObject(name, position, size) {
	addVerbInteraction(Verb::walkVerb, Event([this](EventInfo e) {
		e.player.say("Test");
	}));
}