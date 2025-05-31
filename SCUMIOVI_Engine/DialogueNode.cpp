#include "DialogueNode.hpp"
#include "Game.hpp"

std::vector<DialogueOption>& DialogueNode::getOptions() {
	return options;
}

void DialogueNode::setOptions(std::vector<DialogueOption> options) {
	this->options = options;
}

void DialogueNode::showOptions() {
	Game& game = Game::getInstance();

	// If there are any options, show them
	if (options.size() > 0) {
		game.getUI().createDialogueOptions(*this);
		return;
	}

	// Otherwise, exit the dialogue and return to regular gameplay
	game.getUI().setLayout(game.getUI().getAfterDialogueLayout()->getName());
	game.getCurrentPlayer().enableMovement();
	game.getUI().setHighlightedVerb(nullptr);
	game.getUI().updateUI();
}

void DialogueNode::enterNode() {
	dialogueEvent.execute();
}