#include "GameObject.hpp"

#include "Scene.hpp"
#include <thread>

#include "Vector2.hpp"

// TODO: avoid code repetition with camera moveTo
#define SECONDS_TO_MILLISECONDS 1000
#define UPDATE_FREQUENCY_MS 16 // ~60 times/second
#define EPSILON 0.00001

GameObject::GameObject(
	std::string name,
	Vector2<> position, 
	Vector2<> size) 
{
	if (name == "") {
		//TODO: use some random/UUID generated string, avoid nameless gameObjects
	}

	this->name = name;

	this->position = position;
	this->size = size;
}

GameObject::GameObject(
	std::string name,
	Vector2<> position,
	Vector2<> size,
	Texture* texture) {
	
	this->name = name;

	this->position = position;
	this->size = size;

	textures.push_back(texture);
	this->texture = texture;
}

GameObject::GameObject(
	std::string name,
	Vector2<> position,
	Vector2<> size,
	Animation* animation) {
	this->name = name;

	this->position = position;
	this->size = size;

	animations.push_back(animation);
	this->animation = animation;
}

Vector2<> GameObject::getPosition() {
	return position;
}

void GameObject::setPosition(Vector2<> position) {
	this->position = position;
}

Vector2<> GameObject::getSize() {
	float sizeX = size.x;
	float sizeY = size.y;
	if (sizeX == -1)
		sizeX = texture->getSize().x;
	if (sizeY == -1)
		sizeY = texture->getSize().y;
	return Vector2<>(sizeX, sizeY);
}

void GameObject::setSize(Vector2<> newSize) {
	this->size = size;
}

Texture* GameObject::getTexture() {
	return texture;
}

void GameObject::addTexture(Texture* texture) {
	textures.push_back(texture);
}

void GameObject::setTexture(std::string textureName) {
	if (textureName == "")
		texture = nullptr;
	for (Texture* texture : textures) {
		if (texture->getName() == textureName) {
			this->texture = texture;
		}
		else {
			// TODO: Log error
		}
	}
}

Animation* GameObject::getAnimation() {
	return animation;
}

void GameObject::addAnimation(Animation* animation) {
	animations.push_back(animation);
}

void GameObject::setAnimation(std::string animationName) {
	if (animationName == "")
		animation = nullptr;
	for (Animation* anim : animations) {
		if (anim->getName() == animationName) {
			animation = anim;
		}
	}
}

//Animation* GameObject::getAnimationByName(std::string name) {
//	if (name == "")
//		return nullptr;
//	for (Animation anim : )
//}

//void GameObject::setAnimation(Animation& animation) {
//	this->animation = animation;
//}

Event& GameObject::getMouseOverEvent() {
	return mouseOverEvent;
}

//Event& GameObject::getClickEvent() {
//	return clickEvent;
//}

void GameObject::moveToScene(Scene& scene) {
	currentScene->removeGameObject(*this);
	scene.addGameObject(*this);
}

void GameObject::setScene(Scene* scene) {
	currentScene = scene;
}


//void GameObject::addOverlapEvent(GameObject object, Event event) {
//	overlapEvents.insert(GameObject, event);
//}

//std::map<GameObject, Event> GameObject::getOverlapEvents() {
//
//}

void GameObject::moveToTask(Vector2<> targetPosition, float seconds, Event callback) {
	float nSteps = seconds * SECONDS_TO_MILLISECONDS / UPDATE_FREQUENCY_MS;
	Vector2<> moveStep = (targetPosition - position) / nSteps;
	for (int i = 0; i < nSteps; i++) {
		if (targetPosition.x != currentMovingTarget.x || targetPosition.y != currentMovingTarget.y) {
			//callback.execute();
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_FREQUENCY_MS));
		position = position + moveStep;
	}
	callback.execute();
	moving = false;
}

void GameObject::moveTo(Vector2<> pos, float seconds, Event callback) {
	if (pos.x == currentMovingTarget.x && pos.y == currentMovingTarget.y) {
		return;
	}
	if (seconds <= EPSILON) {
		this->position = pos;
		callback.execute();
		return;
	}
	//std::this_thread::sleep_for(std::chrono::seconds(time));
	moving = true;
	currentMovingTarget = pos;
	auto task = std::bind(&GameObject::moveToTask, this, pos, seconds, callback);
	std::thread backgroundTask(task);
	backgroundTask.detach();
	//std::thread backgroundTask(&moveToTask, pos, seconds, callback);
}

void GameObject::moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> callbacks) {

	while (callbacks.size() < positions.size()) {
		callbacks.push_back(Event());
	}

	for (int i = positions.size()-2; i >= 0; i--) {
		float time = position.distanceTo(positions[i+1]) / speed;
		callbacks[i].addAction([this, positions, i, callbacks] {
			moveToAtSpeed(positions[i+1], 100.0f, callbacks[i]);
		});
	}

	float time = position.distanceTo(positions[0]) / speed;
	moveTo(positions[0], time, callbacks[0]);
}

void GameObject::moveToAtSpeed(Vector2<> pos, float speed, Event callback) {
	float time = position.distanceTo(pos) / speed;
	moveTo(pos, time, callback);
}

void GameObject::addOverlapEvent(GameObject* object, Event event) {
	overlapEvents[object] = event;
}

std::map<GameObject*, Event>& GameObject::getOverlapEvents() {
	return overlapEvents;
}

Vector2<> GameObject::getDirectionVector() {
	if (!moving)
		return Vector2<>();
	return (currentMovingTarget - position).normalized();
}

void GameObject::toggleVisibility() {
	visibility = !visibility;
}

void GameObject::destroy() {
	currentScene->removeGameObject(*this);
}

bool GameObject::isVisible() {
	return visibility;
}

bool GameObject::operator==(const GameObject& o2) const {
	return name == o2.name;
}

bool GameObject::operator!=(const GameObject& o2) const {
	return name != o2.name;
}

std::string& GameObject::getName() {
	return name;
}

void GameObject::setLayer(int layer) {
	this->layer = layer;
}

int GameObject::getLayer() {
	return layer;
}