#include "UIVerbButton.hpp"
#include "Game.hpp"

// TODO: avoid hardcoding highlight color
void UIVerbButton::highlight() {
	auto buttonText = getTextOver();
	//int highlightedColor[4] = { 230, 44, 230, 0xff };
	int highlightedColor[4] = { 255, 255, 255, 0xff };
	Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor, buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
	setTextOver(highlightedText);
}

void UIVerbButton::stopHighlight() {
	setTextOver(getTextOver().toString());
}

// TODO: text format as parameter
UIVerbButton::UIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground) : GameObject(verb.getName() + "Button", position, size, verbBackground), verb(verb) {
	int color[4] = { 125, 44, 125, 0xff };
	Text text = Text(verb.getDisplayName(), 11, color, false, TEXT_CENTERED, true);
	setTextOver(text);
	setTextFormat(text);
	setDefaultVerb(&Verb::anyVerb);
	std::string verbName = verb.getName();
	Game& game = Game::getInstance();
	addVerbInteraction(Verb::anyVerb, Event([verbName] {
		Game& game = Game::getInstance();
		Player& player = game.getCurrentPlayer();
		player.setVerb(verbName);
		game.getUI().updateUI();
		}));
	setMouseOverEvent(Event([this, verbName] {
		highlight();
	}));
	setMouseNotOverEvent(Event([this, verbName](EventInfo e) {
		stopHighlight();
	}));

	//game.getUI().addUIElement(*uiVerbText);

	//return *uiVerbText;
}

Verb& UIVerbButton::getVerb() {
	return verb;
}