#pragma once

#include <vector>
#include <string>
#include <memory>

class Texture;
class GameObject;

class GraphicsComponent {
	friend class GameObject;
private:
	GameObject* owner;

	std::vector<std::shared_ptr<Texture>> textures;
	std::shared_ptr<Texture> texture = nullptr;
	int layer = 0;
	bool visibility = true;

public:
	GraphicsComponent(GameObject* owner);

	std::shared_ptr<Texture> getTexture();
	void addTexture(std::shared_ptr<Texture> texture);
	void setTexture(std::string textureName);

	void toggleVisibility();
	bool isVisible();
	void setLayer(int layer);
	int getLayer();
};
