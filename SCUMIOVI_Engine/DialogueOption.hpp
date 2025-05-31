#pragma once

#include <string>
//#include "DialogueNode.hpp"
#include "Event.hpp"

class DialogueNode;

class DialogueOption {
	std::string text;
	DialogueNode* nextNode;
public:
	DialogueOption(std::string text, DialogueNode* nextNode = nullptr);
	std::string getText();
	void select();
	void setNextNode(DialogueNode& node);
};