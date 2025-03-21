#include "Animation.hpp"

std::vector<Texture>& Animation::getFrames() {
	return frames;
}

Animation::Animation() {}

Animation::Animation(std::string name, std::vector<Texture> frames, bool loop, int frameRate) {
	this->name = name;
	this->frames = frames;
	this->loop = loop;
	this->frameRate = frameRate;
}

Animation::Animation(std::string name, std::string source, Vector2<int> position, Vector2<int> size, int nFrames, int spacing, bool loop, int frameRate) {
	this->name = name;
	for (int i = 0; i < nFrames; i++) {
		Texture texture = Texture("", source, position + i * (size.x + spacing), size);
		frames.push_back(texture);
	}
	this->loop = loop;
	this->frameRate = frameRate;
}

int Animation::getFrameRate() {
	return frameRate;
}

bool Animation::doesLoop() {
	return loop;
}

std::string Animation::getName() {
	return name;
}

Animation Animation::getMirrored(std::string newAnimationName) {
	//std::vector<Texture&> newAnimationFrames;
	//for (Texture frame : frames) {
	//	frame.toggleMirrored();
	//	newAnimationFrames.push_back(frame);
	//}
	Animation newAnimation = Animation(newAnimationName, frames);
	for (Texture& frame : newAnimation.getFrames()) {
		frame.toggleMirrored();
	}
	newAnimation.loop = loop;
	newAnimation.frameRate = frameRate;
	return newAnimation;
}

//void Animation::setFrameRate(int frameRate) {
//	this->frameRate = frameRate;
//}