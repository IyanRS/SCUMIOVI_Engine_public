#include "VerbSyntax.hpp"
#include <cctype>
#include <memory>
#include "GameObject.hpp"
#include "Localization.hpp"

namespace {

// Auto-cases a display name for the position it lands in: lowercase inside
// a sentence ("Give mug to"), capitalized at the start of one. Names whose
// casing is fixed (a leading '=' in the data — see GameObject::setDisplayName)
// are returned exactly as written, so "Stan" survives anywhere in the line.
std::string casedDisplayName(const std::shared_ptr<GameObject>& object, bool atSentenceStart) {
	std::string name = object->getDisplayName();
	if (object->hasFixedCaseDisplayName())
		return name;

	for (char& c : name)
		c = (char)std::tolower((unsigned char)c);
	if (atSentenceStart && !name.empty())
		name[0] = (char)std::toupper((unsigned char)name[0]);
	return name;
}

// Replaces every occurrence of placeholder with the object's display name,
// cased for where it lands in the (already localized) syntax line.
void substitutePlaceholder(std::string& text, const std::string& placeholder, const std::shared_ptr<GameObject>& object) {
	std::string::size_type pos = 0;
	while ((pos = text.find(placeholder, pos)) != std::string::npos) {
		std::string name = casedDisplayName(object, pos == 0);
		text.replace(pos, placeholder.length(), name);
		pos += name.length();
	}
}

} // namespace

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
	else {
		// Templates are matched in the source language, then localized (the
		// translation keeps the $object placeholders) before substitution.
		syntaxToUse = tr(syntaxToUse);
	}

	for (int i = 0; i < nObjects; i++) {
		substitutePlaceholder(syntaxToUse, "$object" + std::to_string(i), nonNullObjects[i]);
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
	else {
		// Localize the chosen template (keeping $selected/$target) before substitution.
		syntaxToUse = tr(syntaxToUse);
	}

	if (selected) {
		substitutePlaceholder(syntaxToUse, "$selected", selected);
	}
	if (target) {
		substitutePlaceholder(syntaxToUse, "$target", target);
	}

	return syntaxToUse;
}

int VerbSyntax::getStringsCount() {
	return (int)syntaxStrings.size();
}