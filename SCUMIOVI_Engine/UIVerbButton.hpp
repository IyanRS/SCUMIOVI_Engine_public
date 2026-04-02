#pragma once
#include "GameObject.hpp"

class UIVerbButton : public GameObject {
private:
	std::shared_ptr<Verb> verb;
public:
	void highlight();
	void stopHighlight();
	UIVerbButton(std::shared_ptr<Verb> verb, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> verbBackground);
	std::shared_ptr<Verb> getVerb();
};