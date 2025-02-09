#pragma once

#include "GameObject.hpp"

#include "Texture.hpp"
#include "DialogueOption.hpp"

class UIDialogueOption : public GameObject {
	DialogueOption& option;
public:
	UIDialogueOption(DialogueOption& option, Vector2<> position, Texture* optionBackground);
	void highlight();
	void stopHighlight();
};