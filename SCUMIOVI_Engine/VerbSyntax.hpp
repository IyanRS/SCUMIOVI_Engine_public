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

	/// <summary>
	/// Returns the current verb syntax by replacing the best fitting syntax string with the selected and target names.
	/// </summary>
	/// <param name="selected"></param>
	/// <param name="target"></param>
	std::string getInfo(GameObject* selected, GameObject* target);

	//std::string getInfo(std::vector<std::string> objects);
	int getStringsCount();
};