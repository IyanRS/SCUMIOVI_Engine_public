#include "VerbSyntax.hpp"
#include <regex>
#include <memory>
#include "GameObject.hpp"

VerbSyntax::VerbSyntax(const VerbSyntax& obj) {
	this->syntaxStrings = obj.syntaxStrings;
}

VerbSyntax::VerbSyntax(std::vector<std::string> syntaxStrings) : syntaxStrings(syntaxStrings) {
}

std::string VerbSyntax::getInfo(std::vector<std::shared_ptr<GameObject>> objects) {
	std::vector<std::shared_ptr<GameObject>> nonNullObjects;

	int nObjects = 0;
	for (auto object : objects) {
		if (object != nullptr) {
			nonNullObjects.push_back(object);
			nObjects++;
		}
	}

	std::string syntaxToUse = "";
	for (const auto& s : syntaxStrings) {
		int occurrences = 0;
		std::string::size_type pos = 0;
		std::string target = "$object";
		while ((pos = s.find(target, pos)) != std::string::npos) {
			++occurrences;
			pos += target.length();
		}

		if (occurrences == nObjects)
		{
			syntaxToUse = s;
			break;
		}
	}
	
	if (syntaxToUse == "") {
		syntaxToUse = "Couldn't find syntax";
	}
	
	for (int i = 0; i < nObjects; i++) {
		syntaxToUse = std::regex_replace(syntaxToUse, std::regex("\\$object" + std::to_string(i)), nonNullObjects[i]->getName());
	}
	return syntaxToUse;
}

std::string VerbSyntax::getInfo(std::shared_ptr<GameObject> selected, std::shared_ptr<GameObject> target) {
	std::string syntaxToUse = "";
	for (const auto& s : syntaxStrings) {
		bool hasSyntaxForSelected = false;
		bool hasSyntaxForTarget = false;
		if (s.find("$selected") != std::string::npos) {
			hasSyntaxForSelected = true;
		}
		if (s.find("$target") != std::string::npos) {
			hasSyntaxForTarget = true;
		}

		if (hasSyntaxForSelected == (selected != nullptr) && 
			hasSyntaxForTarget == (target != nullptr))
		{
			syntaxToUse = s;
			break;
		}
	}
	
	if (syntaxToUse == "") {
		syntaxToUse = "Couldn't find syntax";
	}
	
	if (selected) {
		syntaxToUse = std::regex_replace(syntaxToUse, std::regex("\\$selected"), selected->getName());
	}
	if (target) {
		syntaxToUse = std::regex_replace(syntaxToUse, std::regex("\\$target"), target->getName());
	}
	
	return syntaxToUse;
}

int VerbSyntax::getStringsCount() {
	return (int)syntaxStrings.size();
}