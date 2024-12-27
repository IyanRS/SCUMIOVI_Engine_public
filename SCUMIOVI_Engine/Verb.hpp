#pragma once

#include <string>

class Verb {
	std::string name;
	bool showInUI;
public:
	Verb();
	Verb(std::string name, bool showInUI = true);
	std::string& getName();
};