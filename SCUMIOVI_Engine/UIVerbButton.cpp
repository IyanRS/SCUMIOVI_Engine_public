#include "UIVerbButton.hpp"
#include "Game.hpp"
#include <memory>

void UIVerbButton::highlight() {
	auto buttonText = getTextOver();
	int highlightedColor[4] = { 255, 255, 255, 0xff };
	Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor, buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
	setTextOver(highlightedText);
}

void UIVerbButton::stopHighlight() {
	setTextOver(getTextOver().toString());
}

UIVerbButton::UIVerbButton(std::shared_ptr<Verb> verb, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> verbBackground)
    : UIElement(verb->getName() + "Button", position, size, verbBackground), verb(verb)
{
	int color[4] = { 125, 44, 125, 0xff };
	Text text = Text(verb->getDisplayName(), 11, color, false, TEXT_CENTERED, true);
	setTextOver(text);
	setTextFormat(text);
	
	Game& game = Game::getInstance();
	setDefaultVerb(game.anyVerb);
	
	std::string verbName = verb->getName();
	addVerbInteraction(game.anyVerb, Event([verbName] {
		Game& game = Game::getInstance();
		auto player = game.getCurrentPlayer();
		if (player) {
			player->setVerb(verbName);
			game.getUI().updateUI();
		}
	}));

	setMouseOverEvent(Event([this] {
		highlight();
	}));
	setMouseNotOverEvent(Event([this](EventInfo e) {
		stopHighlight();
	}));
}

std::shared_ptr<Verb> UIVerbButton::getVerb() {
	return verb;
}