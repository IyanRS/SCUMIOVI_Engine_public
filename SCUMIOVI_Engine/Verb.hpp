#pragma once

#include <string>

class Verb {
	std::string name;
public:
	Verb();
	Verb(std::string name);
	std::string& getName();
};