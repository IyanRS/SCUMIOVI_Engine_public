#include "MovementComponent.hpp"
#include "GameObject.hpp"
#include "Common.hpp"

MovementComponent::MovementComponent(GameObject* owner) : owner(owner) {}

void MovementComponent::update(float deltaTime) {
	if (moving) {
		moveElapsed += deltaTime;
		if (moveElapsed >= moveDuration) {
			owner->setPosition(moveDest);
			moving   = false;
			owner->setMovementAnimation(Vector2<>(0, 0));
			moveCallback.execute();
		} else {
			float t  = moveElapsed / moveDuration;
			owner->setPosition(moveStart + (moveDest - moveStart) * t);
			owner->setMovementAnimation(getDirectionVector());
		}
	}
}

void MovementComponent::moveTo(Vector2<> pos, float seconds, Event callback) {
	if (moving && moveDest.distanceTo(pos) < EPSILON)
		return;

	if (owner->getPosition().distanceTo(pos) < EPSILON) {
		callback.execute();
		return;
	}

	if (seconds <= EPSILON) {
		owner->setPosition(pos);
		owner->setMovementAnimation(Vector2<>(0, 0));
		callback.execute();
		return;
	}

	moving       = true;
	moveStart    = owner->getPosition();
	moveDest     = pos;
	moveDuration = seconds;
	moveElapsed  = 0.0f;
	moveCallback = callback;

	owner->setMovementAnimation(getDirectionVector());
}

void MovementComponent::moveToAtSpeed(Vector2<> pos, float speed, Event callback) {
	float time = owner->getPosition().distanceTo(pos) / speed;
	moveTo(pos, time, callback);
}

void MovementComponent::moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> callbacks) {
	while (callbacks.size() < positions.size()) {
		callbacks.push_back(Event());
	}

	for (int i = (int)positions.size() - 2; i >= 0; i--) {
		callbacks[i].addAction([this, positions, i, callbacks, speed] {
			moveToAtSpeed(positions[i + 1], speed, callbacks[i + 1]);
		});
	}

	moveToAtSpeed(positions[0], speed, callbacks[0]);
}

Vector2<> MovementComponent::getDirectionVector() {
	if (!moving)
		return Vector2<>();
	return (moveDest - owner->getPosition()).normalized();
}

Vector2<> MovementComponent::getLastMovementDirection() const {
	return lastMovementDirection;
}

void MovementComponent::setLastMovementDirection(Vector2<> dir) {
	lastMovementDirection = dir;
}
