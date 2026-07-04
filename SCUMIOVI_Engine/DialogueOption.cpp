#include "DialogueOption.hpp"
#include "DialogueNode.hpp"
#include "Game.hpp"
#include "Localization.hpp"
#include <memory>

DialogueOption::DialogueOption(std::string text, std::shared_ptr<DialogueNode> nextNode) {
	this->text = text;
	this->nextNode = nextNode;
}

std::string DialogueOption::getText() {
	return tr(text);
}

void DialogueOption::select() {
	Game& game = Game::getInstance();
	game.getUI().clearDialogueOptions();
	if (nextNode) {
		nextNode->enterNode();
	}
}

void DialogueOption::setNextNode(std::shared_ptr<DialogueNode> node) {
	nextNode = node;
}

void DialogueOption::setCondition(std::function<bool()> cond) {
	condition = cond;
}

bool DialogueOption::isVisible() const {
	return condition ? condition() : true;
}