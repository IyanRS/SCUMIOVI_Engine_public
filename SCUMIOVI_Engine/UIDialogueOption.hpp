#pragma once

#include "UIElement.hpp"

#include "Texture.hpp"
#include "DialogueOption.hpp"

class UIDialogueOption : public UIElement {
	std::shared_ptr<DialogueOption> option;
public:
	UIDialogueOption(std::shared_ptr<DialogueOption> option, Vector2<> position, std::shared_ptr<Texture> optionBackground);
	void highlight();
	void stopHighlight();
};