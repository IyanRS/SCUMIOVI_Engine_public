#pragma once

#include <string>

enum TextOverMode {
	TEXT_ON_TOP,
	TEXT_CENTERED,
	TEXT_TO_LEFT
};

class Text {
	std::string textString;
	int size;
	int color[4];
	bool outline = false;
	bool shadow = false;
	TextOverMode textOverMode;
public:
	// Empty text constructor
	Text();
	Text(std::string textString, int size, int color[4], bool outline = false, TextOverMode textOverMode = TEXT_ON_TOP, bool shadow = false);
	Text(std::string textString, Text& text);
	std::string toString();
	int getSize();
	void getColor(int[4]);
	bool hasOutline();
	TextOverMode getTextOverMode();
	bool hasShadow();
};