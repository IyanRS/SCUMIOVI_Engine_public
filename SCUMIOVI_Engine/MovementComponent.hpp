#pragma once

#include "Vector2.hpp"
#include "Event.hpp"
#include <vector>

class GameObject;

class MovementComponent {
	friend class GameObject;
private:
	GameObject* owner;

	bool moving = false;
	Vector2<> moveStart;
	Vector2<> moveDest;
	float moveDuration = 0.0f;
	float moveElapsed = 0.0f;
	Event moveCallback;
	Vector2<> lastMovementDirection = Vector2<>(0, 0);

public:
	MovementComponent(GameObject* owner);

	void update(float deltaTime);

	void moveTo(Vector2<> pos, float seconds = 0, Event callback = Event());
	void moveToAtSpeed(Vector2<> pos, float speed, Event callback = Event());
	void moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> callbacks = std::vector<Event>());

	// Cancels any in-progress movement immediately, without firing the
	// completion callback, and resets to the idle animation for the last
	// facing direction. Used when e.g. a save is loaded mid-walk, so the
	// object doesn't keep interpolating toward a now-irrelevant destination.
	void stop();
	bool isMoving() const;

	Vector2<> getDirectionVector();
	Vector2<> getLastMovementDirection() const;
	void setLastMovementDirection(Vector2<> dir);
};
