#include "Scene.hpp"
#include <thread>

#include "Camera.hpp"
#include <algorithm>

Scene::Scene() {
	debugPoints = std::list<Vector2<>>();
	debugLines = std::list<std::array<Vector2<>, 2>>();
}

//bool Scene::gameObjectsComparator(GameObject& o1, GameObject& o2) {
//	return o1.getLayer() > o2.getLayer();
//}

void Scene::addGameObject(GameObject& gameObject) {
	gameObjects.push_back(&gameObject);
	gameObject.setScene(this);
	std::sort(gameObjects.begin(), gameObjects.end(), [this](GameObject* o1, GameObject* o2) {
		return o1->getLayer() < o2->getLayer();
	});
}

void Scene::removeGameObject(GameObject& gameObject) {
	gameObjects.erase(std::find(gameObjects.begin(), gameObjects.end(), &gameObject));
	gameObject.setScene(nullptr);
}

Camera& Scene::getCamera() {
	return camera;
}

std::vector<GameObject*> Scene::getGameObjects() {
	return gameObjects;
}

WalkableArea* Scene::getWalkableArea() {
	return walkableArea;
}

void Scene::setWalkableArea(WalkableArea* walkableArea) {
	this->walkableArea = walkableArea;
}

GameObject* Scene::getGameObject(std::string gameObjectName) {
	if (gameObjectName == "")
		return nullptr;
	for (GameObject* gameObject : gameObjects) {
		if (gameObject->getName() == gameObjectName) {
			return gameObject;
		}
	}
	return nullptr;
}

void Scene::addDebugPoint(Vector2<> point) {
	debugPoints.push_back(point);
}

void Scene::addDebugLine(Vector2<> lineStart, Vector2<> lineEnd) {
	std::array<Vector2<>, 2> line;
	line[0] = lineStart;
	line[1] = lineEnd;
	debugLines.push_back(line);
}

std::list<Vector2<>> Scene::getDebugPoints() {
	return debugPoints;
}

std::list<std::array<Vector2<>, 2>> Scene::getDebugLines() {
	return debugLines;
}