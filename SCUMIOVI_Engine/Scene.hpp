#pragma once

#include <list>

class Camera;
#include "Camera.hpp"
#include "GameObject.hpp"
#include "WalkableArea.hpp"

#include <string>

#include <array>
#include <vector>

class Scene {
	std::vector<std::shared_ptr<GameObject>> gameObjects;
	Camera camera;
	WalkableArea* walkableArea = nullptr;

	//bool gameObjectsComparator(GameObject& o1, GameObject& o2);

	// Debug elements to be shown
	// TODO: group into a DebugElement class and use inheritance for dots and lines (and maybe polygon)
	std::list<Vector2<>> debugPoints;
	std::list<std::array<Vector2<>, 2>> debugLines;

public:
	Scene();
	Camera& getCamera();
	//void moveCameraTo(Vector2<> position, float seconds, Event callback);
	void addGameObject(std::shared_ptr<GameObject> gameObject);
	void removeGameObject(const std::shared_ptr<GameObject>& gameObject);
	std::shared_ptr<GameObject> addBackground(Scene& scene, std::string source, Vector2<int> size);
	std::vector<std::shared_ptr<GameObject>> getGameObjects();
	// TODO: maybe a list of walkableAreas is better, it would allow scenarios
	// with multiple separated areas.
	WalkableArea* getWalkableArea();
	void setWalkableArea(WalkableArea* walkableArea);
	std::shared_ptr<GameObject> getGameObject(std::string);
	void addDebugPoint(Vector2<>);
	void addDebugLine(Vector2<>, Vector2<>);
	std::list<Vector2<>> getDebugPoints();
	std::list<std::array<Vector2<>, 2>> getDebugLines();
	// Called once per frame by the game loop
	void update(float deltaTime);
};