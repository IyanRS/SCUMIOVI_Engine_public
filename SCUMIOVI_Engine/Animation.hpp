#pragma once

#include <vector>
#include <string>
#include "Texture.hpp"
#include <memory>

class Animation {
	std::vector<Texture> frames;
	int frameRate = 10; // TODO: frameRate and loop in constructor
	bool loop = true;
	std::string name;
public:
	Animation();
	Animation(std::string name, std::vector<Texture> frames, bool loop = true, int frameRate = 10);
	Animation(std::string name, std::string source, Vector2<int> position, Vector2<int> size, int nFrames, int spacing = 0, bool loop = true, int frameRate = 10);
	std::vector<Texture>& getFrames();
	//void setFrameRate(int frameRate);
	int currentFrame = 0;
	int getFrameRate();
	bool doesLoop();
	std::string getName();
	std::shared_ptr<Animation> getMirrored(std::string newAnimationName);
};