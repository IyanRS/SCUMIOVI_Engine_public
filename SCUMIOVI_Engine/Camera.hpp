#pragma once

#include "Vector2.hpp"
#include "Event.hpp"

class GameObject;
#include "GameObject.hpp"

class Camera {
	Vector2<> position;
	Vector2<> size;
	std::weak_ptr<GameObject> objectToFollow;
	void followObject();

	// Delta-time movement
	bool isMoving = false;
	Vector2<> moveStart;
	Vector2<> moveTarget;
	float moveDuration = 0.0f;  // total seconds for this move
	float moveElapsed  = 0.0f;  // seconds elapsed so far
	Event moveCallback;

public:
	Camera();
	Vector2<> getPosition();
	Vector2<> getSize();
	void setSize(Vector2<> size);
	void moveTo(Vector2<> position, float seconds, Event callback = Event());
	void setObjectToFollow(std::shared_ptr<GameObject> object);
	void stopFollowingObject();
	// Called once per frame by Scene::update()
	void update(float deltaTime);
};