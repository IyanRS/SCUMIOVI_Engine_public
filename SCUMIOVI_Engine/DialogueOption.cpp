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
	
	//if (nextNode != nullptr) {
	//	event.addAction(Delay(1));
	//	event = event + Event([this](EventInfo e) {
	//		e.game.getUI().createDialogueOptions(*nextNode); 
	//	});
	//}
	event.execute();
}

void DialogueOption::finishDialogue(std::string layoutName) {
	Game& game = Game::getInstance();

	if (nextNode != nullptr) {
		game.getUI().createDialogueOptions(*nextNode);
		return;
	}

	game.getUI().setLayout(layoutName);
	game.getCurrentPlayer().enableMovement();
	game.getUI().setHighlightedVerb(nullptr);
	game.getUI().updateUI();
}

void DialogueOption::setNextNode(DialogueNode& node) {
	nextNode = &node;
}