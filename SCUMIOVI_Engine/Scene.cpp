#include "Scene.hpp"

#include "Camera.hpp"
#include <algorithm>

#include "Verb.hpp"

static const int DEFAULT_BACKGROUND_LAYER = -100;

Scene::Scene() {
	debugPoints = std::list<Vector2<>>();
	debugLines = std::list<std::array<Vector2<>, 2>>();
}

//bool Scene::gameObjectsComparator(GameObject& o1, GameObject& o2) {
//	return o1.getLayer() > o2.getLayer();
//}

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObjects.push_back(gameObject);
	gameObject->setScene(this);
	std::sort(gameObjects.begin(), gameObjects.end(), [this](const std::shared_ptr<GameObject>& o1, const std::shared_ptr<GameObject>& o2) {
		return o1->getLayer() < o2->getLayer();
	});
}

std::shared_ptr<GameObject> Scene::addBackground(Scene& scene, std::string source, Vector2<int> size) {
	// TODO: handle pointers memory for textures (deferred to next phase)
	Texture* backgroundTexture = new Texture("", source, Vector2<int>(), size);
	auto background = std::make_shared<GameObject>("background", Vector2<>(0, 0), Vector2<>(-1, -1), backgroundTexture);
	background->setLayer(DEFAULT_BACKGROUND_LAYER);

	scene.addGameObject(background);
	return background;
}

void Scene::removeGameObject(const std::shared_ptr<GameObject>& gameObject) {
	auto it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
	if (it != gameObjects.end()) {
		(*it)->setScene(nullptr);
		gameObjects.erase(it);
	}
}

Camera& Scene::getCamera() {
	return camera;
}

std::vector<std::shared_ptr<GameObject>> Scene::getGameObjects() {
	return gameObjects;
}

WalkableArea* Scene::getWalkableArea() {
	return walkableArea;
}

void Scene::setWalkableArea(WalkableArea* walkableArea) {
	this->walkableArea = walkableArea;
}

std::shared_ptr<GameObject> Scene::getGameObject(std::string gameObjectName) {
	if (gameObjectName == "")
		return nullptr;
	for (const auto& gameObject : gameObjects) {
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

void Scene::update(float deltaTime) {
	// Update all game objects (copy vector to avoid iterator invalidation if destroy() is called)
	std::vector<std::shared_ptr<GameObject>> objectsCopy = gameObjects;
	for (const auto& obj : objectsCopy) {
		if (obj != nullptr)
			obj->update(deltaTime);
	}
	// Update the camera
	camera.update(deltaTime);
}