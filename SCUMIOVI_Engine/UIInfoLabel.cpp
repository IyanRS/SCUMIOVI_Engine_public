#include "UIInfoLabel.hpp"
#include "Game.hpp"

UIInfoLabel::UIInfoLabel(std::string name, Vector2<> position, Text format)
	: UIElement(name, position, Vector2<>(0, 0)) {
	setTextOver(format);
	setTextFormat(format);
}

void UIInfoLabel::refresh() {
	UIElement::refresh();

	Game& game = Game::getInstance();
	auto player = game.getCurrentPlayer();
	if (!player) return;

	auto currentVerb = player->getVerb();
	if (!currentVerb) return;

	setTextOver(currentVerb->getInfo(player->getSelectedObject(), player->getTargetObject()));
}
