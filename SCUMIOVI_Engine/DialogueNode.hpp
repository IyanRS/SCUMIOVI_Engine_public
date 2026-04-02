#pragma once

#include "DialogueOption.hpp"
#include <string>
#include <vector>
#include <memory>
#include "Event.hpp"

class DialogueNode {
	std::vector<std::shared_ptr<DialogueOption>> options;
	Event dialogueEvent;

public:
	DialogueNode(Event event) : dialogueEvent(event) {};
	std::vector<std::shared_ptr<DialogueOption>>& getOptions();
	void setOptions(std::vector<std::shared_ptr<DialogueOption>> options);
	void showOptions();
	void enterNode();
};