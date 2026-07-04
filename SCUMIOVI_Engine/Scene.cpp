#include "Scene.hpp"
#include <SDL3/SDL.h>
#include <algorithm>
#include <memory>

Scene::Scene() {}

void Scene::setName(const std::string& newName) {
	name = newName;
}

const std::string& Scene::getName() const {
	return name;
}

Camera& Scene::getCamera() {
	return camera;
}

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject) {
	gameObjects.push_back(gameObject);
	gameObject->setScene(shared_from_this());
	objectsDirty = true;
}

void Scene::removeGameObject(const std::shared_ptr<GameObject>& gameObject) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
	objectsDirty = true;
}

void Scene::markLayersDirty() {
	objectsDirty = true;
}

std::shared_ptr<GameObject> Scene::addBackground(std::string source, Vector2<int> size) {
	std::shared_ptr<Texture> backgroundTexture = std::make_shared<Texture>("background", source, Vector2<int>(0, 0), size);
	std::shared_ptr<GameObject> background = std::make_shared<GameObject>("background", Vector2<float>(0, 0), Vector2<float>((float)size.x, (float)size.y), backgroundTexture);
	background->setLayer(-100);
	addGameObject(background);
	return background;
}

std::vector<std::shared_ptr<GameObject>> Scene::getGameObjects() {
	if (objectsDirty) {
		std::sort(gameObjects.begin(), gameObjects.end(), [](const std::shared_ptr<GameObject>& a, const std::shared_ptr<GameObject>& b) {
			return a->getLayer() < b->getLayer();
		});
		objectsDirty = false;
	}
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

void Scene::checkOverlaps(std::shared_ptr<GameObject> gameObject1, std::shared_ptr<GameObject> gameObject2) {
	auto& overlapEvents = gameObject1->getOverlapEvents();
	auto it = overlapEvents.find(gameObject2);
	if (it == overlapEvents.end()) {
		return;
	}

	SDL_Rect rectObj1 = { (int)gameObject1->getPosition().x, (int)gameObject1->getPosition().y, (int)gameObject1->getSize().x, (int)gameObject1->getSize().y };
	SDL_Rect rectObj2 = { (int)gameObject2->getPosition().x, (int)gameObject2->getPosition().y, (int)gameObject2->getSize().x, (int)gameObject2->getSize().y };

	int currentOverlappingIndex = -1;
	for (int i = 0; i < (int)currentlyOverlapping.size(); i++) {
		auto& overlap = currentlyOverlapping[i];
		if (overlap[0] == gameObject1 && overlap[1] == gameObject2) {
			currentOverlappingIndex = i;
			break;
		}
	}

	if (SDL_HasRectIntersection(&rectObj1, &rectObj2)) {
		if (currentOverlappingIndex == -1) {
			it->second.execute();
			currentlyOverlapping.push_back({ gameObject1, gameObject2 });
		}
	}
	else {
		if (currentOverlappingIndex != -1) {
			currentlyOverlapping.erase(currentlyOverlapping.begin() + currentOverlappingIndex);
		}
	}
}

void Scene::update(float deltaTime) {
	camera.update(deltaTime);
	for (int i = 0; i < (int)gameObjects.size(); i++) {
		gameObjects[i]->update(deltaTime);
	}

	// Overlap events (moved out of the renderer so gameplay logic lives here).
	// Iterate a snapshot because an overlap event may add/remove objects.
	auto snapshot = gameObjects;
	for (const auto& object : snapshot) {
		if (object == nullptr) continue;
		for (const auto& object2 : snapshot) {
			if (object2 == nullptr || object == object2) continue;
			checkOverlaps(object, object2);
		}
	}
}