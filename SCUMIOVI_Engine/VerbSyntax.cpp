#include "VerbSyntax.hpp"
#include <regex>

//VerbSyntax::VerbSyntax() {}

VerbSyntax::VerbSyntax(const VerbSyntax& obj) {
	this->syntaxStrings = obj.syntaxStrings;
}

VerbSyntax::VerbSyntax(std::vector<std::string> syntaxStrings) : syntaxStrings(syntaxStrings) {
	//this->syntaxStrings = syntaxStrings;
}

#include "GameObject.hpp"

std::string VerbSyntax::getInfo(std::vector<GameObject*> objects) {
	//TODO: improve this code (maybe allow to use "$object1" twice, 
	// use only "$object1" and "$object3" without "$object2", etc.)

	std::vector<GameObject*> nonNullObjects;

	int nObjects = 0;
	for (auto object : objects) {
		if (object != nullptr) {
			nonNullObjects.push_back(object);
			nObjects++;
		}
	}

	std::string syntaxToUse = "";
	for (auto s : syntaxStrings) {
		// Find how many "$object" placeholder the current string has
		int occurrences = 0;
		std::string::size_type pos = 0;
		std::string target = "$object";
		while ((pos = s.find(target, pos)) != std::string::npos) {
			++occurrences;
			pos += target.length();
		}

		// If the number of "$object" placeholders is the same as the number of objects, 
		// we will use that syntax for the verb information
		if (occurrences == nObjects)
		{
			syntaxToUse = s;
			break;
		}
	}
	if (syntaxToUse == "") {
		// TODO: throw exception
		syntaxToUse = "Couldn't find syntax";
	}
	//syntaxToUse = std::regex_replace(syntaxToUse, std::regex("\\$object0"), "Test");
	for (int i = 0; i < nObjects; i++) {
		syntaxToUse = std::regex_replace(syntaxToUse, std::regex("\\$object" + std::to_string(i)), nonNullObjects[i]->getName());
	}
	return syntaxToUse;
}

int VerbSyntax::getStringsCount() {
	return syntaxStrings.size();
}