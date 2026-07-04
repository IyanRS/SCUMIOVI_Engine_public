#pragma once

#include <string>
#include <memory>
#include <functional>
//#include "DialogueNode.hpp"
#include "Event.hpp"

class DialogueNode;

class DialogueOption {
	std::string text;
	std::shared_ptr<DialogueNode> nextNode;
	std::function<bool()> condition;
public:
	DialogueOption(std::string text, std::shared_ptr<DialogueNode> nextNode = nullptr);
	std::string getText();
	void select();
	void setNextNode(std::shared_ptr<DialogueNode> node);
	void setCondition(std::function<bool()> cond);
	bool isVisible() const;
};