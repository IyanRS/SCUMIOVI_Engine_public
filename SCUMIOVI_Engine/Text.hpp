#pragma once

#include <string>

enum TextOverMode {
	TEXT_ON_TOP,
	TEXT_CENTERED
};

class Text {
	std::string textString;
	int size;
	int color[4];
	bool outline = false;
	TextOverMode textOverMode;
public:
	// Empty text constructor
	Text();
	Text(std::string textString, int size, int color[4], bool outline, TextOverMode textOverMode = TEXT_ON_TOP);
	Text(std::string textString, Text& text);
	std::string toString();
	int getSize();
	void getColor(int[4]);
	bool hasOutline();
	TextOverMode getTextOverMode();
};