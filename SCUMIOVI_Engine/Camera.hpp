#pragma once

#include "Vector2.hpp"
#include "Event.hpp"
#include "CameraZone.hpp"

#include <vector>

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

	// Camera zones (optional — replaces followObject per-zone)
	std::vector<CameraZone> zones;
	int currentZoneIndex = -1;
	bool useZones = false;
	// True for the whole duration of a zone crossing (camera pan + forced
	// nudge walk + player input lock) — while true, zones aren't
	// re-evaluated, so a crossing can't retrigger itself mid-flight.
	bool zoneTransitioning = false;
	void beginZoneTransition(std::shared_ptr<GameObject> target, int newZoneIndex, float direction);

public:
	Camera();
	Vector2<> getPosition();
	Vector2<> getSize();
	void setSize(Vector2<> size);
	void moveTo(Vector2<> position, float seconds, Event callback = Event());
	void setObjectToFollow(std::shared_ptr<GameObject> object);
	void stopFollowingObject();
	void setZones(const std::vector<CameraZone>& newZones);
	void clearZones();
	// Called once per frame by Scene::update()
	void update(float deltaTime);
};