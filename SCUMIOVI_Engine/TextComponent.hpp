#pragma once

#include "Text.hpp"
#include "Event.hpp"
#include <string>

class GameObject;

class TextComponent {
	friend class GameObject;
private:
	GameObject* owner;

	float sayTimer = 0.0f;
	Event sayCallback;
	Text textOver;
	Text textFormat;

public:
	TextComponent(GameObject* owner);

	void update(float deltaTime);

	Text& getTextOver();
	void setTextOver(std::string textOver);
	void setTextOver(Text& textOver);
	void say(Text& textOver, float seconds = 0, Event callback = Event());
	void say(std::string text, float seconds = 0, Event callback = Event());
	void setTextFormat(Text& text);
};
