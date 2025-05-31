#pragma once

#include "DialogueOption.hpp"
#include <string>
#include <vector>
#include "Event.hpp"

class DialogueNode {
	std::vector<DialogueOption> options;
	Event dialogueEvent;

public:
	DialogueNode(Event event) : dialogueEvent(event) {};
	std::vector<DialogueOption>& getOptions();
	void setOptions(std::vector<DialogueOption> options);
	void showOptions();
	void enterNode();
};