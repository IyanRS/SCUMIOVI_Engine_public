#pragma once

#include <string>
//#include "DialogueNode.hpp"
#include "Event.hpp"

class DialogueNode;

class DialogueOption {
	std::string text;
	DialogueNode* nextNode;
	Event event;
public:
	DialogueOption(std::string text, Event event);
	std::string getText();
	void select();
	void setNextNode(DialogueNode& node);
	void finishDialogue(std::string layoutName);
};