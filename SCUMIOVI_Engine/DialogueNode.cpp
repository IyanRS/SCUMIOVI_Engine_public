#include "DialogueNode.hpp"

//DialogueNode::DialogueNode(std::string text) {
//
//}

std::vector<DialogueOption>& DialogueNode::getOptions() {
	return options;
}

void DialogueNode::setOptions(std::vector<DialogueOption> options) {
	this->options = options;
}