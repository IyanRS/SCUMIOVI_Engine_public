#pragma once
#include "UIElement.hpp"

class UIVerbButton : public UIElement {
private:
	std::shared_ptr<Verb> verb;
public:
	void highlight();
	void stopHighlight();
	UIVerbButton(std::shared_ptr<Verb> verb, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> verbBackground);
	std::shared_ptr<Verb> getVerb();
	// Re-resolves the label from the verb's (localized) display name, so a
	// locale switch retitles the button on the next UI refresh.
	void refresh() override;
};