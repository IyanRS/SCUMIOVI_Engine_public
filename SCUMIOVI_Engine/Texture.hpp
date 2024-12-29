#pragma once

#include <string>
#include "Vector2.hpp"

class Texture {
	std::string source;

	// If we are using an atlas as source
	Vector2<int> position;
	Vector2<int> size;
	std::string name;
	bool mirror = false;
	int color[4] = {-1, -1, -1, -1};
public:
	Texture(std::string name = "", std::string source = "", Vector2<int> position = Vector2<int>(), Vector2<int> size = Vector2<int>(-1, -1));
	Texture(std::string name, int color[4], Vector2<int> position = Vector2<int>(), Vector2<int> size = Vector2<int>(-1, -1));
	std::string& getSource();
	Vector2<int> getPosition();
	Vector2<int> getSize();
	void setSize(Vector2<int> newSize);
	std::string getName();
	bool isMirrored();
	void toggleMirrored();
	Texture getMirrored(std::string newTextureName);
	void getColor(int color[4]);
};