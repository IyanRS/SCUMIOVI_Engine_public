#include "UIDialogueOption.hpp"
#include "DialogueOption.hpp"
#include "Game.hpp"
#include <memory>

void UIDialogueOption::highlight() {
	auto buttonText = getTextOver();
	int highlightedColor[4] = { 255, 255, 255, 0xff };
	Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor, buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
	setTextOver(highlightedText);
}

void UIDialogueOption::stopHighlight() {
	setTextOver(getTextOver().toString());
}

UIDialogueOption::UIDialogueOption(std::shared_ptr<DialogueOption> option, Vector2<> position, std::shared_ptr<Texture> optionBackground) 
    : GameObject("option", position, Vector2<>((float)option->getText().length() * 5, 11), optionBackground), option(option) 
{
	int color[4] = { 125, 44, 125, 0xff };
	Text text = Text(option->getText(), 11, color, false, TEXT_TO_LEFT, true);
	setTextOver(text);
	setTextFormat(text);
	
	Game& game = Game::getInstance();
	setDefaultVerb(game.anyVerb);
	
	addVerbInteraction(game.anyVerb, Event([this] {
		this->option->select();
	}));
	setMouseOverEvent(Event([this] {
		highlight();
	}));
	setMouseNotOverEvent(Event([this](EventInfo e) {
		stopHighlight();
	}));
}