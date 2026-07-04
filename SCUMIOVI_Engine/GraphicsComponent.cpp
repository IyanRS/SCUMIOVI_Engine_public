#include "GraphicsComponent.hpp"
#include "GameObject.hpp"
#include "Texture.hpp"
#include <memory>

GraphicsComponent::GraphicsComponent(GameObject* owner) : owner(owner) {}

std::shared_ptr<Texture> GraphicsComponent::getTexture() {
	return texture;
}

void GraphicsComponent::addTexture(std::shared_ptr<Texture> t) {
	textures.push_back(t);
}

void GraphicsComponent::setTexture(std::string textureName) {
	if (textureName == "") {
		texture = nullptr;
		return;
	}
	for (auto t : textures) {
		if (t->getName() == textureName) {
			this->texture = t;
			return;
		}
	}
}

void GraphicsComponent::toggleVisibility() {
	visibility = !visibility;
}

void GraphicsComponent::setVisible(bool visible) {
	visibility = visible;
}

bool GraphicsComponent::isVisible() {
	return visibility;
}

void GraphicsComponent::setLayer(int layer) {
	this->layer = layer;
}

int GraphicsComponent::getLayer() {
	return layer;
}
