#include "DialogueOption.hpp"

#include "Game.hpp"

DialogueOption::DialogueOption(std::string text, Event event) {
	this->text = text;
	this->event = event;
}

std::string DialogueOption::getText() {
	return text;
}

void DialogueOption::select() {
	Game& game = Game::getInstance();
	game.getUI().clearDialogueOptions();
	//game.getUI().createDialogueOptions(*nextNode);
	event.execute();
}

void DialogueOption::setNextNode(DialogueNode& node) {
	nextNode = &node;
}