#include "Texture.hpp"

std::string& Texture::getSource() {
	return source;
}

Vector2<int> Texture::getPosition() {
	return position;
}

Vector2<int> Texture::getSize() {
	return size;
}

void Texture::setSize(Vector2<int> newSize) {
	size = newSize;
}

Texture::Texture(std::string name, std::string source, Vector2<int> position, Vector2<int> size) {
	this->name = name;
	this->source = source;
	this->position = position;
	this->size = size;
	mirror = false;
}

std::string Texture::getName() {
	return name;
}

bool Texture::isMirrored() {
	return mirror;
}

void Texture::toggleMirrored() {
	mirror = !mirror;
}

Texture Texture::getMirrored(std::string newTextureName) {
	Texture newTexture = Texture(newTextureName, source, position, size);
	newTexture.toggleMirrored();
	return newTexture;
}