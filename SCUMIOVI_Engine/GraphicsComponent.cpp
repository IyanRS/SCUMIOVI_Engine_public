#include "GraphicsComponent.hpp"
#include "GameObject.hpp"
#include "Texture.hpp"

GraphicsComponent::GraphicsComponent(GameObject* owner) : owner(owner) {}

Texture* GraphicsComponent::getTexture() {
	return texture;
}

void GraphicsComponent::addTexture(Texture* t) {
	textures.push_back(t);
}

void GraphicsComponent::setTexture(std::string textureName) {
	if (textureName == "") {
		texture = nullptr;
		return;
	}
	for (Texture* t : textures) {
		if (t->getName() == textureName) {
			this->texture = t;
			return;
		}
	}
}

void GraphicsComponent::toggleVisibility() {
	visibility = !visibility;
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
