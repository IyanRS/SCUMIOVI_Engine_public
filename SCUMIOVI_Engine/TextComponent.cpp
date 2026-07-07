#include "TextComponent.hpp"
#include "GameObject.hpp"

#define TEXT_SPEED 10.0f

TextComponent::TextComponent(GameObject* owner) : owner(owner) {}

void TextComponent::update(float deltaTime) {
	if (sayTimer > 0.0f) {
		sayTimer -= deltaTime;
		if (sayTimer <= 0.0f) {
			sayTimer = 0.0f;
			Text empty;
			setTextOver(empty);
			owner->animator.stopTalking();
			sayCallback.execute();
			sayCallback = Event();  // clear to avoid double-fire
		}
	}
}

Text& TextComponent::getTextOver() {
	return textOver;
}

void TextComponent::setTextOver(Text& textOver) {
	this->textOver = textOver;
}

void TextComponent::setTextOver(std::string text) {
	Text newTextOver = Text(text, textFormat);
	setTextOver(newTextOver);
}

void TextComponent::setTextFormat(Text& text) {
	this->textFormat = text;
}

void TextComponent::say(Text& textToSay, float seconds, Event callback) {
	if (seconds <= 0) {
		seconds = (float)textToSay.toString().size() / TEXT_SPEED;
		if (seconds < 0.5f) seconds = 0.5f;  // minimum display time
	}
	setTextOver(textToSay);
	sayTimer    = seconds;
	sayCallback = callback;
	owner->animator.startTalking();
}

void TextComponent::say(std::string text, float seconds, Event callback) {
	Text textToSay = Text(text, textFormat);
	say(textToSay, seconds, callback);
}

void TextComponent::stopSaying() {
	if (sayTimer <= 0.0f) return;
	sayTimer = 0.0f;
	sayCallback = Event();
	Text empty;
	setTextOver(empty);
	owner->animator.stopTalking();
}
