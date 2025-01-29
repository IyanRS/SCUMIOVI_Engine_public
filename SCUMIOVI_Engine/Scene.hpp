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
	std::vector<GameObject*> gameObjects;
	Camera camera;
	WalkableArea* walkableArea;

	//bool gameObjectsComparator(GameObject& o1, GameObject& o2);

	// Debug elements to be shown
	// TODO: group into a DebugElement class and use inheritance for dots and lines (and maybe polygon)
	std::list<Vector2<>> debugPoints;
	std::list<std::array<Vector2<>, 2>> debugLines;

public:
	Scene();
	Camera& getCamera();
	//void moveCameraTo(Vector2<> position, float seconds, Event callback);
	void addGameObject(GameObject& gameObject);
	void removeGameObject(GameObject& gameObject);
	GameObject& addBackground(Scene& scene, std::string source, Vector2<int> size);
	std::vector<GameObject*> getGameObjects();
	// TODO: maybe a list of walkableAreas is better, it would allow scenarios
	// with multiple separated areas.
	WalkableArea* getWalkableArea();
	void setWalkableArea(WalkableArea* walkableArea);
	GameObject* getGameObject(std::string);
	void addDebugPoint(Vector2<>);
	void addDebugLine(Vector2<>, Vector2<>);
	std::list<Vector2<>> getDebugPoints();
	std::list<std::array<Vector2<>, 2>> getDebugLines();
};