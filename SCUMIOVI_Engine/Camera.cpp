#include "Camera.hpp"

#include "Common.hpp"

#define EPSILON 0.00001f

Camera::Camera() {
	size = Vector2<>(RENDER_WIDTH, RENDER_HEIGHT);
}

void Camera::followObject() {
	if (auto target = objectToFollow.lock()) {
		position = target->getPosition();
	}
}

void Camera::setObjectToFollow(std::shared_ptr<GameObject> object) {
	objectToFollow = object;
}

void Camera::stopFollowingObject() {
	objectToFollow.reset();
}

Vector2<> Camera::getPosition() {
	return position;
}

Vector2<> Camera::getSize() {
	return size;
}

void Camera::setSize(Vector2<> size) {
	this->size = size;
}

void Camera::moveTo(Vector2<> pos, float seconds, Event callback) {
	// Already heading there — ignore duplicate calls
	if (isMoving && moveTarget.x == pos.x && moveTarget.y == pos.y)
		return;

	if (seconds <= EPSILON) {
		// Instantaneous
		position = pos;
		callback.execute();
		return;
	}

	isMoving     = true;
	moveStart    = position;
	moveTarget   = pos;
	moveDuration = seconds;
	moveElapsed  = 0.0f;
	moveCallback = callback;
}

// -------------------------------------------------------------------------
// update() — called once per frame by Scene::update(dt).
// Advances the camera's lerp-based movement without any sleeping.
// -------------------------------------------------------------------------
void Camera::update(float deltaTime) {
	// If a follow target is set, snap to it (overrides any moveTo)
	if (auto target = objectToFollow.lock()) {
		followObject();
		return;
	}

	if (!isMoving)
		return;

	moveElapsed += deltaTime;

	if (moveElapsed >= moveDuration) {
		// Arrived
		position = moveTarget;
		isMoving = false;
		moveCallback.execute();
	} else {
		// Linear interpolation
		float t = moveElapsed / moveDuration;
		position = moveStart + (moveTarget - moveStart) * t;
	}
}