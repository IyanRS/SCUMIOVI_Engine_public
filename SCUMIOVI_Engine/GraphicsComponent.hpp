#pragma once

#include <vector>
#include <string>

class Texture;
class GameObject;

class GraphicsComponent {
	friend class GameObject;
private:
	GameObject* owner;

	std::vector<Texture*> textures;
	Texture* texture = nullptr;
	int layer = 0;
	bool visibility = true;

public:
	GraphicsComponent(GameObject* owner);

	Texture* getTexture();
	void addTexture(Texture* texture);
	void setTexture(std::string textureName);

	void toggleVisibility();
	bool isVisible();
	void setLayer(int layer);
	int getLayer();
};
