#pragma once

#include "DialogueOption.hpp"
#include <string>
#include <vector>
#include "Event.hpp"

class DialogueNode {
	std::vector<DialogueOption> options;

public:
	//DialogueNode(std::string text);
	std::vector<DialogueOption>& getOptions();
	void setOptions(std::vector<DialogueOption> options);
};