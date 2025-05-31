#include "DialogueOption.hpp"

#include "Game.hpp"

DialogueOption::DialogueOption(std::string text, DialogueNode* nextNode) {
	this->text = text;
	this->nextNode = nextNode;
}

std::string DialogueOption::getText() {
	return text;
}

void DialogueOption::select() {
	Game& game = Game::getInstance();
	game.getUI().clearDialogueOptions();
	nextNode->enterNode();
}

void DialogueOption::setNextNode(DialogueNode& node) {
	nextNode = &node;
}