#pragma once

#include "GameObject.hpp"

#include "Texture.hpp"
#include "DialogueOption.hpp"

class UIDialogueOption : public GameObject {
	std::shared_ptr<DialogueOption> option;
public:
	UIDialogueOption(std::shared_ptr<DialogueOption> option, Vector2<> position, std::shared_ptr<Texture> optionBackground);
	void highlight();
	void stopHighlight();
};