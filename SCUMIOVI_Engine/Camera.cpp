#include "Camera.hpp"
#include <thread>

#include "Common.hpp"

#define SECONDS_TO_MILLISECONDS 1000
#define CAMERA_UPDATE_FREQUENCY_MS 16 // ~60 times/second
#define EPSILON 0.00001

Camera::Camera() {
	size = Vector2<>(RENDER_WIDTH, RENDER_HEIGHT);
}

void Camera::followObject() {
	if (objectToFollow == nullptr) {
		return;
	}
	position = objectToFollow->getPosition();
}

void Camera::setObjectToFollow(GameObject& object) {
	objectToFollow = &object;
}

void Camera::stopFollowingObject() {
	objectToFollow = nullptr;
}

Vector2<> Camera::getPosition() {
	return position;
}

Vector2<> Camera::getSize() {
	return size;
}

void Camera::setSize(Vector2<> position) {
	this->size = size;
}

void Camera::moveToTask(Vector2<> targetPosition, float seconds, Event callback) {
	float nSteps = seconds * SECONDS_TO_MILLISECONDS / CAMERA_UPDATE_FREQUENCY_MS;
	Vector2<> moveStep = (targetPosition - this->position) / nSteps;
	for (int i = 0; i < nSteps; i++) {
		if (targetPosition.x != currentMovingTarget.x || targetPosition.y != currentMovingTarget.y) {
			//callback.execute();
			return;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CAMERA_UPDATE_FREQUENCY_MS));
		this->position = this->position + moveStep;
	}
	callback.execute();
}

void Camera::moveTo(Vector2<> pos, float seconds, Event callback) {
	if (pos.x == currentMovingTarget.x && pos.y == currentMovingTarget.y)
		return;
	if (seconds <= EPSILON) {
		this->position = pos;
		callback.execute();
		return;
	}
	currentMovingTarget = pos;
	//std::this_thread::sleep_for(std::chrono::seconds(time));
	auto task = std::bind(&Camera::moveToTask, this, pos, seconds, callback);
	std::thread backgroundTask(task);
	backgroundTask.detach();
	//std::thread backgroundTask(&moveToTask, pos, seconds, callback);
}