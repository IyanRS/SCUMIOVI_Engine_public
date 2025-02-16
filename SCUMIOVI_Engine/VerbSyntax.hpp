#pragma once

#include <vector>
#include <string>

//#include "GameObject.hpp"
class GameObject;

class VerbSyntax {
	std::vector<std::string> syntaxStrings;
public:
	//VerbSyntax();

	VerbSyntax(const VerbSyntax& obj); // Copy constructor

	VerbSyntax(std::vector<std::string> syntaxStrings = {});
	std::string getInfo(std::vector<GameObject*> objects);
	//std::string getInfo(std::vector<std::string> objects);
	int getStringsCount();
};