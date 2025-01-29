#pragma once
#include "GameObject.hpp"

class UIVerbButton : public GameObject {
private:
	Verb& verb;
public:
	void highlight();
	void stopHighlight();
	UIVerbButton(Verb& verb, Vector2<> position, Vector2<> size, Texture* verbBackground);
	Verb& getVerb();
};