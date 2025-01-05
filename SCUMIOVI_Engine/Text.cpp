#include "Text.hpp"

Text::Text() {
	textString = "";
	for (int i = 0; i < 4; i++)
		color[i] = 0;
	size = 0;
	textOverMode = TEXT_ON_TOP;
}

Text::Text(std::string textString, int size, int color[4], bool outline, TextOverMode textOverMode, bool shadow) {
	this->textString = textString;
	this->size = size;
	for (int i = 0; i < 4; i++)
		this->color[i] = color[i];
	this->outline = outline;
	this->textOverMode = textOverMode;
	this->shadow = shadow;
}

Text::Text(std::string textString, Text& text) {
	this->textString = textString;
	this->size = text.size;
	for (int i = 0; i < 4; i++)
		this->color[i] = text.color[i];
	this->outline = text.outline;
	this->textOverMode = text.textOverMode;
	this->shadow = text.shadow;
}

std::string Text::toString() {
	return textString;
}

int Text::getSize() {
	return size;
}

void Text::getColor(int color[4]) {
	for (int i = 0; i < 4; i++)
		color[i] = this->color[i];
}

bool Text::hasOutline() {
	return outline;
}

TextOverMode Text::getTextOverMode() {
	return textOverMode;
}

bool Text::hasShadow() {
	return shadow;
}