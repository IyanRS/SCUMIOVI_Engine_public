#include "Scene.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <memory>

Scene::Scene() {}

Camera& Scene::getCamera() {
	return camera;
}

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObjects.push_back(gameObject);
	gameObject->setScene(shared_from_this());
}

void Scene::removeGameObject(const std::shared_ptr<GameObject>& gameObject) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
}

std::shared_ptr<GameObject> Scene::addBackground(std::string source, Vector2<int> size) {
	std::shared_ptr<Texture> backgroundTexture = std::make_shared<Texture>("background", source, Vector2<int>(0, 0), size);
	std::shared_ptr<GameObject> background = std::make_shared<GameObject>("background", Vector2<float>(0, 0), Vector2<float>((float)size.x, (float)size.y), backgroundTexture);
	background->setLayer(-100);
	addGameObject(background);
	return background;
}

std::vector<std::shared_ptr<GameObject>> Scene::getGameObjects() {
	std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b) {
		return a->getLayer() < b->getLayer();
	});
	return gameObjects;
}

WalkableArea* Scene::getWalkableArea() {
	return walkableArea.get();
}

void Scene::setWalkableArea(std::unique_ptr<WalkableArea> newWalkableArea) {
	walkableArea = std::move(newWalkableArea);
}

std::shared_ptr<GameObject> Scene::getGameObject(std::string name) {
	for (auto object : gameObjects) {
		if (object->getName() == name) {
			return object;
		}
	}
	return nullptr;
}

void Scene::addDebugPoint(Vector2<> point) {
	debugPoints.push_back(point);
}

void Scene::addDebugLine(Vector2<> p1, Vector2<> p2) {
	debugLines.push_back({p1, p2});
}

std::list<Vector2<>> Scene::getDebugPoints() {
	return debugPoints;
}

std::list<std::array<Vector2<>, 2>> Scene::getDebugLines() {
	return debugLines;
}

void Scene::update(float deltaTime) {
	camera.update(deltaTime);
	for (int i = 0; i < (int)gameObjects.size(); i++) {
		gameObjects[i]->update(deltaTime);
	}
}