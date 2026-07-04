#include "DialogueNode.hpp"
#include "Game.hpp"
#include <memory>

void DialogueNode::setDialogueEvent(Event event) {
	dialogueEvent = event;
}

std::vector<std::shared_ptr<DialogueOption>>& DialogueNode::getOptions() {
	return options;
}

void DialogueNode::setOptions(std::vector<std::shared_ptr<DialogueOption>> options) {
	this->options = options;
}

void DialogueNode::showOptions() {
	Game& game = Game::getInstance();

	bool anyVisible = false;
	for (auto& opt : options) {
		if (opt->isVisible()) {
			anyVisible = true;
			break;
		}
	}

	if (anyVisible) {
		game.getUI().createDialogueOptions(*this);
		return;
	}

	game.getUI().setLayout(game.getUI().getAfterDialogueLayout()->getName());
	game.getCurrentPlayer()->enableMovement();
	game.getUI().setHighlightedVerb(nullptr);
	game.getUI().updateUI();
}

void DialogueNode::enterNode() {
	dialogueEvent.execute();
}