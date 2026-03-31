#include "GameObject.hpp"

#include "Scene.hpp"

#include "Vector2.hpp"

#include "Common.hpp"

#define EPSILON 0.001f
#define TEXT_SPEED 10.0f

// ---------------------------------------------------------------------------
// Constructors
// ---------------------------------------------------------------------------

GameObject::GameObject(
	std::string name,
	Vector2<> position,
	Vector2<> size)
{
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

// ---------------------------------------------------------------------------
// Basic accessors
// ---------------------------------------------------------------------------

Vector2<> GameObject::getPosition() {
	return position;
}

void GameObject::setPosition(Vector2<> position) {
	this->position = position;
}

Vector2<> GameObject::getSize() {
	float sizeX = size.x;
	float sizeY = size.y;
	if (sizeX == -1 && texture != nullptr)
		sizeX = texture->getSize().x;
	if (sizeY == -1 && texture != nullptr)
		sizeY = texture->getSize().y;
	return Vector2<>(sizeX, sizeY);
}

void GameObject::setSize(Vector2<> newSize) {
	this->size = newSize;
}

Texture* GameObject::getTexture() {
	return texture;
}

void GameObject::addTexture(Texture* texture) {
	textures.push_back(texture);
}

void GameObject::setTexture(std::string textureName) {
	if (textureName == "") {
		texture = nullptr;
		return;
	}
	for (Texture* t : textures) {
		if (t->getName() == textureName) {
			this->texture = t;
			return;
		}
	}
}

Animation* GameObject::getAnimation() {
	return animation;
}

void GameObject::addAnimation(Animation* animation) {
	animations.push_back(animation);
}

Animation* GameObject::getAnimationByName(std::string name) {
	if (name == "")
		return nullptr;
	for (Animation* anim : animations) {
		if (anim->getName() == name) {
			return anim;
		}
	}
	return nullptr;
}

void GameObject::setAnimation(std::string animationName) {
	if (animationName == "") {
		animation = nullptr;
		return;
	}
	for (Animation* anim : animations) {
		if (anim->getName() == animationName) {
			animation = anim;
		}
	}
}

// ---------------------------------------------------------------------------
// playAnimation — non-looping one-shot: records what to return to after the
// animation finishes. The Renderer already increments currentFrame each
// render call, so we just need to know the total frame count and fire the
// callback whenever that count is reached (checked in update()).
// ---------------------------------------------------------------------------
void GameObject::playAnimation(std::string animationName, Event callback) {
	if (animationName == "") {
		animation = nullptr;
		return;
	}
	for (Animation* anim : animations) {
		if (anim->getName() == animationName) {
			if (!anim->doesLoop()) {
				// Remember what animation to return to once the one-shot finishes
				animReturnTo    = animation;
				anim->currentFrame = 0;
				animation       = anim;
				// Total displayable frames = nFrames * frameRate render-ticks
				animReturnFrame = (int)anim->getFrames().size() * anim->getFrameRate();
				animCallback    = callback;
			} else {
				setAnimation(anim->getName());
			}
			return;
		}
	}
}

// ---------------------------------------------------------------------------
// Movement animation helpers (unchanged logic, no threads needed)
// ---------------------------------------------------------------------------

bool GameObject::horizontalAnimationsDefined() {
	return moveRightAnimation != nullptr && moveLeftAnimation != nullptr;
}

bool GameObject::verticalAnimationsDefined() {
	return moveUpAnimation != nullptr && moveDownAnimation != nullptr;
}

bool GameObject::horizontalIdleDefined() {
	return idleRightAnimation != nullptr && idleLeftAnimation != nullptr;
}

bool GameObject::verticalIdleDefined() {
	return idleUpAnimation != nullptr && idleDownAnimation != nullptr;
}

void GameObject::setIdleAnimation(Vector2<> lastDir) {
	if ((abs(lastDir.x) > abs(lastDir.y) || !verticalIdleDefined()) && horizontalIdleDefined()) {
		animation = (lastDir.x > 0) ? idleRightAnimation : idleLeftAnimation;
	} else if (verticalIdleDefined()) {
		animation = (lastDir.y > 0) ? idleUpAnimation : idleDownAnimation;
	}
}

void GameObject::setMovementAnimations(
	Animation* moveRight, Animation* moveLeft,
	Animation* moveUp,    Animation* moveDown,
	Animation* idleRight, Animation* idleLeft,
	Animation* idleUp,    Animation* idleDown)
{
	moveRightAnimation = moveRight;
	moveLeftAnimation  = moveLeft;
	moveUpAnimation    = moveUp;
	moveDownAnimation  = moveDown;
	idleRightAnimation = idleRight;
	idleLeftAnimation  = idleLeft;
	idleUpAnimation    = idleUp;
	idleDownAnimation  = idleDown;
}

void GameObject::setMovementAnimation(Vector2<> movementDirection) {
	if (movementDirection.x == 0 && movementDirection.y == 0) {
		setIdleAnimation(lastMovementDirection);
		return;
	}

	lastMovementDirection = movementDirection;

	if ((abs(movementDirection.x) > abs(movementDirection.y)
		|| !verticalAnimationsDefined()) && horizontalAnimationsDefined()) {
		animation = (movementDirection.x > 0) ? moveRightAnimation : moveLeftAnimation;
	} else if (verticalAnimationsDefined()) {
		animation = (movementDirection.y > 0) ? moveDownAnimation : moveUpAnimation;
	}
}

// ---------------------------------------------------------------------------
// moveTo — non-blocking: records state, returns immediately.
// ---------------------------------------------------------------------------
void GameObject::moveTo(Vector2<> pos, float seconds, Event callback) {
	// Already heading to the exact same destination — ignore
	if (moving && moveDest.distanceTo(pos) < EPSILON)
		return;

	if (position.distanceTo(pos) < EPSILON) {
		callback.execute();
		return;
	}

	if (seconds <= EPSILON) {
		// Instantaneous
		position = pos;
		setMovementAnimation(Vector2<>(0, 0));
		callback.execute();
		return;
	}

	moving       = true;
	moveStart    = position;
	moveDest     = pos;
	moveDuration = seconds;
	moveElapsed  = 0.0f;
	moveCallback = callback;

	setMovementAnimation(getDirectionVector());
}

void GameObject::moveToAtSpeed(Vector2<> pos, float speed, Event callback) {
	float time = position.distanceTo(pos) / speed;
	moveTo(pos, time, callback);
}

void GameObject::moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> callbacks) {
	while (callbacks.size() < positions.size()) {
		callbacks.push_back(Event());
	}

	// Chain each waypoint: when we arrive at positions[i], move to positions[i+1]
	for (int i = (int)positions.size() - 2; i >= 0; i--) {
		float time = position.distanceTo(positions[i + 1]) / speed;
		callbacks[i].addAction([this, positions, i, callbacks, speed] {
			moveToAtSpeed(positions[i + 1], speed, callbacks[i + 1]);
		});
	}

	moveToAtSpeed(positions[0], speed, callbacks[0]);
}

// ---------------------------------------------------------------------------
// say — records how long the text should show; update() clears it.
// ---------------------------------------------------------------------------
void GameObject::say(Text& textToSay, float seconds, Event callback) {
	if (seconds <= 0) {
		seconds = (float)textToSay.toString().size() / TEXT_SPEED;
		if (seconds < 0.5f) seconds = 0.5f;  // minimum display time
	}
	setTextOver(textToSay);
	sayTimer    = seconds;
	sayCallback = callback;
}

void GameObject::say(std::string text, float seconds, Event callback) {
	Text textToSay = Text(text, textFormat);
	say(textToSay, seconds, callback);
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
	return (moveDest - position).normalized();
}

// ---------------------------------------------------------------------------
// update() — called every frame by Scene::update(dt). Advances movement,
// say-timer, and one-shot animation without any threads.
// ---------------------------------------------------------------------------
void GameObject::update(float deltaTime) {

	// 1) Movement interpolation
	if (moving) {
		moveElapsed += deltaTime;
		if (moveElapsed >= moveDuration) {
			position = moveDest;
			moving   = false;
			setMovementAnimation(Vector2<>(0, 0));
			moveCallback.execute();
		} else {
			float t  = moveElapsed / moveDuration;
			position = moveStart + (moveDest - moveStart) * t;
			setMovementAnimation(getDirectionVector());
		}
	}

	// 2) Say timer
	if (sayTimer > 0.0f) {
		sayTimer -= deltaTime;
		if (sayTimer <= 0.0f) {
			sayTimer = 0.0f;
			Text empty;
			setTextOver(empty);
			sayCallback.execute();
			sayCallback = Event();  // clear to avoid double-fire
		}
	}

	// 3) One-shot animation done?
	if (animReturnFrame >= 0 && animation != nullptr) {
		if (animation->currentFrame >= animReturnFrame - 1) {
			// Animation has finished; restore previous animation
			animation      = animReturnTo;
			animReturnTo   = nullptr;
			animReturnFrame = -1;
			animCallback.execute();
			animCallback = Event();
		}
	}
}

// ---------------------------------------------------------------------------
// Visibility, scene, tags, state, text helpers
// ---------------------------------------------------------------------------

void GameObject::moveToScene(Scene& scene) {
	if (currentScene != nullptr)
		currentScene->removeGameObject(*this);
	scene.addGameObject(*this);
}

void GameObject::setScene(Scene* scene) {
	currentScene = scene;
}

Scene* GameObject::getScene() {
	return currentScene;
}

void GameObject::toggleVisibility() {
	visibility = !visibility;
}

void GameObject::destroy() {
	if (currentScene != nullptr)
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

Text& GameObject::getTextOver() {
	return textOver;
}

void GameObject::setTextOver(Text& textOver) {
	this->textOver = textOver;
}

void GameObject::setTextOver(std::string text) {
	Text newTextOver = Text(text, textFormat);
	setTextOver(newTextOver);
}

void GameObject::setState(int state) {
	this->state = state;
}

int GameObject::getState() {
	return state;
}

void GameObject::setTextFormat(Text& text) {
	this->textFormat = text;
}

void GameObject::addTag(std::string tag) {
	objectTags.insert(tag);
}

bool GameObject::hasTag(std::string tag) {
	return objectTags.contains(tag);
}

VerticalDirection GameObject::getCurrentVerticalDirection() {
	return lastMovementDirection.y < 0 ? UP : DOWN;
}

HorizontalDirection GameObject::getCurrentHorizontalDirection() {
	return lastMovementDirection.x < 0 ? LEFT : RIGHT;
}