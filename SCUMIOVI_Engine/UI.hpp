#pragma once

#include "Texture.hpp"

#include <vector>
#include <memory>
#include "GameObject.hpp"
#include "UIVerbButton.hpp"
#include "UILayout.hpp"

class UI {
	UILayout* currentLayout = nullptr;

	std::vector<std::unique_ptr<UILayout>> layouts;

	UILayout* dialogueLayout = nullptr;
	UILayout* afterDialogueLayout = nullptr;

	// A layout shown on top of the current one (e.g. a pause/options menu),
	// without replacing it. See openOverlay/closeOverlay.
	UILayout* overlayLayout = nullptr;
	bool overlayPausesGame = false;

public:
	std::vector<std::shared_ptr<GameObject>>& getUIElements();
	// Draw-ordered, absolute-positioned list of the active layout's element
	// tree, followed by the overlay layout's (if any) so it renders on top and
	// receives clicks first.
	std::vector<std::shared_ptr<GameObject>> getRenderElements();

	// Shows a previously created layout on top of the current one. If
	// pauseGame is true (the default), the scene stops updating and world
	// objects stop receiving clicks/hover while the overlay is active — the
	// base layout and the overlay itself keep working normally.
	void openOverlay(std::string name, bool pauseGame = true);
	void closeOverlay();
	UILayout* getOverlay();
	bool isGamePaused();

	void updateUI();

	void setHighlightedVerb(std::shared_ptr<Verb> verb);

	void toggleVisibility();
	std::shared_ptr<GameObject> getUIElement(std::string name);
	void setLayout(std::string name);
	void createLayout(std::string name);
	UILayout& getLayout(std::string name);
	UILayout* getLayout();
	void createDialogueOptions(DialogueNode& dialogueNode);
	void clearDialogueOptions();
	void incrementInventoryIndex(int increment);
	int getInventoryIndex();
	void setDialogueLayout(UILayout* layout);
	UILayout* getDialogueLayout();
	void setAfterDialogueLayout(UILayout* layout);
	UILayout* getAfterDialogueLayout();
};