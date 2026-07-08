#pragma once

#include <list>

class Camera;
#include "Camera.hpp"
#include "GameObject.hpp"
#include "WalkableArea.hpp"

#include <string>

#include <array>
#include <vector>

class Scene : public std::enable_shared_from_this<Scene> {
	std::vector<std::shared_ptr<GameObject>> gameObjects;
	// Set when the object set or an object's layer changes, so getGameObjects()
	// re-sorts by layer only when needed instead of every frame.
	bool objectsDirty = true;
	Camera camera;
	std::unique_ptr<WalkableArea> walkableArea = nullptr;
	std::string name;

	std::list<Vector2<>> debugPoints;
	std::list<std::array<Vector2<>, 2>> debugLines;

	// Pairs of objects currently intersecting, so an overlap event fires once
	// on enter rather than every frame.
	std::vector<std::array<std::shared_ptr<GameObject>, 2>> currentlyOverlapping;
	void checkOverlaps(std::shared_ptr<GameObject> gameObject1, std::shared_ptr<GameObject> gameObject2);

public:
	Scene();
	void setName(const std::string& newName);
	const std::string& getName() const;
	Camera& getCamera();
	//void moveCameraTo(Vector2<> position, float seconds, Event callback);
	void addGameObject(std::shared_ptr<GameObject> gameObject);
	void removeGameObject(const std::shared_ptr<GameObject>& gameObject);
	std::shared_ptr<GameObject> addBackground(std::string source, Vector2<int> size);
	std::vector<std::shared_ptr<GameObject>> getGameObjects();
	// Flags the object list for re-sorting; called when a member's layer changes.
	void markLayersDirty();
	// TODO: maybe a list of walkableAreas is better, it would allow scenarios
	// with multiple separated areas.
	WalkableArea* getWalkableArea();
	void setWalkableArea(std::unique_ptr<WalkableArea> walkableArea);
	std::shared_ptr<GameObject> getGameObject(std::string);
	void addDebugPoint(Vector2<>);
	void addDebugLine(Vector2<>, Vector2<>);
	std::list<Vector2<>>& getDebugPoints();
	std::list<std::array<Vector2<>, 2>>& getDebugLines();
	// Called once per frame by the game loop
	void update(float deltaTime);
};