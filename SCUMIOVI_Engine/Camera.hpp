#pragma once

#include "Vector2.hpp"
#include "Event.hpp"

class GameObject;
#include "GameObject.hpp"

class Camera {
	Vector2<> position;
	Vector2<> size;
	GameObject* objectToFollow;
	void followObject();
	Vector2<> currentMovingTarget = Vector2<>(-INFINITY, -INFINITY);
	void moveToTask(Vector2<> position, float seconds = 0, Event callback = Event());
public:
	Camera();
	Vector2<> getPosition();
	Vector2<> getSize();
	void setSize(Vector2<> size);
	void moveTo(Vector2<> position, float seconds, Event callback = Event());
	void setObjectToFollow(GameObject& object);
	void stopFollowingObject();
};