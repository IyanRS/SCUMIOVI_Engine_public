#include "GameObject.hpp"
#include "Scene.hpp"
#include "Vector2.hpp"
#include "Common.hpp"
#include <memory>
#include <cmath>

#define EPSILON 0.001f
#define TEXT_SPEED 10.0f

GameObject::GameObject(
	std::string name,
	Vector2<> position,
	Vector2<> size) : movement(this), text(this), animator(this), graphics(this)
{
	this->name = name;
	this->position = position;
	this->size = size;
}

GameObject::GameObject(
	std::string name,
	Vector2<> position,
	Vector2<> size,
	std::shared_ptr<Texture> texture) : movement(this), text(this), animator(this), graphics(this) {

	this->name = name;
	this->position = position;
	this->size = size;

	graphics.textures.push_back(texture);
	graphics.texture = texture;
}

GameObject::GameObject(
	std::string name,
	Vector2<> position,
	Vector2<> size,
	std::shared_ptr<Animation> animation) : movement(this), text(this), animator(this), graphics(this) {
	this->name = name;
	this->position = position;
	this->size = size;

	animator.animations.push_back(animation);
	animator.animation = animation;
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
	if (sizeX == -1 && graphics.texture != nullptr)
		sizeX = (float)graphics.texture->getSize().x;
	if (sizeY == -1 && graphics.texture != nullptr)
		sizeY = (float)graphics.texture->getSize().y;
	return Vector2<>(sizeX, sizeY);
}

void GameObject::setSize(Vector2<> newSize) {
	this->size = newSize;
}

std::shared_ptr<Texture> GameObject::getTexture() {
	return graphics.getTexture();
}

void GameObject::addTexture(std::shared_ptr<Texture> texture) {
	graphics.addTexture(texture);
}

void GameObject::setTexture(std::string textureName) {
	graphics.setTexture(textureName);
}

std::shared_ptr<Animation> GameObject::getAnimation() {
	return animator.getAnimation();
}

void GameObject::addAnimation(std::shared_ptr<Animation> animation) {
	animator.addAnimation(animation);
}

std::shared_ptr<Animation> GameObject::getAnimationByName(std::string name) {
	return animator.getAnimationByName(name);
}

void GameObject::setAnimation(std::string animationName) {
	animator.setAnimation(animationName);
}

void GameObject::playAnimation(std::string animationName, Event callback) {
	animator.playAnimation(animationName, callback);
}

bool GameObject::horizontalAnimationsDefined() {
	return animator.horizontalAnimationsDefined();
}

bool GameObject::verticalAnimationsDefined() {
	return animator.verticalAnimationsDefined();
}

bool GameObject::horizontalIdleDefined() {
	return animator.horizontalIdleDefined();
}

bool GameObject::verticalIdleDefined() {
	return animator.verticalIdleDefined();
}

void GameObject::setIdleAnimation(Vector2<> lastDir) {
	animator.setIdleAnimation(lastDir);
}

void GameObject::setMovementAnimations(
	std::shared_ptr<Animation> moveRight, std::shared_ptr<Animation> moveLeft,
	std::shared_ptr<Animation> moveUp,    std::shared_ptr<Animation> moveDown,
	std::shared_ptr<Animation> idleRight, std::shared_ptr<Animation> idleLeft,
	std::shared_ptr<Animation> idleUp,    std::shared_ptr<Animation> idleDown)
{
	animator.setMovementAnimations(moveRight, moveLeft, moveUp, moveDown, idleRight, idleLeft, idleUp, idleDown);
}

void GameObject::setMovementAnimation(Vector2<> movementDirection) {
	animator.setMovementAnimation(movementDirection);
}

void GameObject::moveTo(Vector2<> pos, float seconds, Event callback) {
	movement.moveTo(pos, seconds, callback);
}

void GameObject::moveToAtSpeed(Vector2<> pos, float speed, Event callback) {
	movement.moveToAtSpeed(pos, speed, callback);
}

void GameObject::moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> callbacks) {
	movement.moveToAtSpeed(positions, speed, callbacks);
}

void GameObject::say(Text& textToSay, float seconds, Event callback) {
	text.say(textToSay, seconds, callback);
}

void GameObject::say(std::string textToSay, float seconds, Event callback) {
	text.say(textToSay, seconds, callback);
}

void GameObject::addOverlapEvent(std::shared_ptr<GameObject> object, Event event) {
	overlapEvents[object] = event;
}

std::map<std::shared_ptr<GameObject>, Event>& GameObject::getOverlapEvents() {
	return overlapEvents;
}

Vector2<> GameObject::getDirectionVector() {
	return movement.getDirectionVector();
}

void GameObject::update(float deltaTime) {
	movement.update(deltaTime);
	text.update(deltaTime);
	animator.update(deltaTime);
}

void GameObject::moveToScene(std::shared_ptr<Scene> scene) {
	if (currentScene != nullptr)
		currentScene->removeGameObject(shared_from_this());
	scene->addGameObject(shared_from_this());
}

void GameObject::setScene(std::shared_ptr<Scene> scene) {
	currentScene = scene;
}

std::shared_ptr<Scene> GameObject::getScene() {
	return currentScene;
}

void GameObject::toggleVisibility() {
	graphics.toggleVisibility();
}

void GameObject::destroy() {
	if (currentScene != nullptr)
		currentScene->removeGameObject(shared_from_this());
}

bool GameObject::isVisible() {
	return graphics.isVisible();
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
	graphics.setLayer(layer);
}

int GameObject::getLayer() {
	return graphics.getLayer();
}

Text& GameObject::getTextOver() {
	return text.getTextOver();
}

void GameObject::setTextOver(Text& textOver) {
	text.setTextOver(textOver);
}

void GameObject::setTextOver(std::string textOver) {
	text.setTextOver(textOver);
}

void GameObject::setState(int state) {
	this->state = state;
}

int GameObject::getState() {
	return state;
}

void GameObject::setExamineText(const std::string& text) {
	examineText = text;
}

const std::string& GameObject::getExamineText() const {
	return examineText;
}

void GameObject::setTextFormat(Text& format) {
	text.setTextFormat(format);
}

void GameObject::addTag(std::string tag) {
	objectTags.insert(tag);
}

bool GameObject::hasTag(std::string tag) {
	return objectTags.contains(tag);
}

VerticalDirection GameObject::getCurrentVerticalDirection() {
	return movement.lastMovementDirection.y < 0 ? UP : DOWN;
}

HorizontalDirection GameObject::getCurrentHorizontalDirection() {
	return movement.lastMovementDirection.x < 0 ? LEFT : RIGHT;
}