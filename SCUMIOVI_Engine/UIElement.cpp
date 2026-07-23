#include "UIElement.hpp"
#include "Game.hpp"
#include "Localization.hpp"

// --- UIElement base ----------------------------------------------------------
UIElement::UIElement(std::string name, Vector2<> position, Vector2<> size)
	: GameObject(name, position, size), offset(position) {
}

UIElement::UIElement(std::string name, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> texture)
	: GameObject(name, position, size, texture), offset(position) {
}

void UIElement::addChild(std::shared_ptr<GameObject> child) {
	children.push_back(child);
}

std::vector<std::shared_ptr<GameObject>>& UIElement::getChildren() {
	return children;
}

Vector2<> UIElement::getOffset() const {
	return offset;
}

void UIElement::setOffset(Vector2<> newOffset) {
	offset = newOffset;
}

UIAnchor UIElement::getAnchor() const {
	return anchor;
}

void UIElement::setAnchor(UIAnchor newAnchor) {
	anchor = newAnchor;
}

void UIElement::refresh() {
	// Plain GameObject children (e.g. inventory item icons) have no refresh
	// concept; only recurse into actual UIElements.
	for (auto& child : children) {
		if (auto uiChild = std::dynamic_pointer_cast<UIElement>(child)) {
			uiChild->refresh();
		}
	}
}

// --- UIImage -----------------------------------------------------------------
UIImage::UIImage(std::string name, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> texture)
	: UIElement(name, position, size, texture) {
}

// --- UILabel -----------------------------------------------------------------
UILabel::UILabel(std::string name, Vector2<> position, Text text)
	: UIElement(name, position, Vector2<>(0, 0)) {
	setTextOver(text);
	setTextFormat(text);
}

void UILabel::setLocalizedText(std::string text) {
	sourceText = text;
	setTextOver(tr(text));
}

void UILabel::refresh() {
	UIElement::refresh();
	if (!sourceText.empty()) {
		setTextOver(tr(sourceText));
	}
}

// --- UISaveSlotLabel ---------------------------------------------------------
UISaveSlotLabel::UISaveSlotLabel(std::string name, Vector2<> position, Text format, int slot)
	: UIElement(name, position, Vector2<>(0, 0)), slot(slot) {
	setTextOver(format);
	setTextFormat(format);
}

void UISaveSlotLabel::refresh() {
	UIElement::refresh();
	setTextOver(Game::getInstance().getSaveTimestamp(slot));
}

// --- UIButton ----------------------------------------------------------------
UIButton::UIButton(std::string name, Vector2<> position, Vector2<> size,
	std::shared_ptr<Texture> texture, Text label, Event onClick, Event onHover)
	: UIElement(name, position, size, texture), onHoverEvent(onHover) {
	setTextOver(label);
	setTextFormat(label);

	// Any click on the button runs onClick: bind it to the engine's "any" verb
	// so it fires regardless of the player's currently selected verb.
	Game& game = Game::getInstance();
	setDefaultVerb(game.anyVerb);
	addVerbInteraction(game.anyVerb, onClick);

	setMouseOverEvent(onHover);
}

void UIButton::setLocalizedText(std::string text) {
	sourceText = text;
	setTextOver(tr(text));
}

void UIButton::setHoverHighlight(bool enabled) {
	if (!enabled) {
		setMouseOverEvent(onHoverEvent);
		setMouseNotOverEvent(Event());
		return;
	}

	// Same treatment as UIVerbButton: white text while hovered, back to the
	// format colour on leave.
	setMouseOverEvent(onHoverEvent + Event([this] {
		auto buttonText = getTextOver();
		int highlightedColor[4] = { 255, 255, 255, 0xff };
		Text highlightedText = Text(buttonText.toString(), buttonText.getSize(), highlightedColor,
			buttonText.hasOutline(), buttonText.getTextOverMode(), buttonText.hasShadow());
		setTextOver(highlightedText);
	}));
	setMouseNotOverEvent(Event([this] {
		setTextOver(getTextOver().toString());
	}));
}

void UIButton::refresh() {
	UIElement::refresh();
	if (!sourceText.empty()) {
		setTextOver(tr(sourceText));
	}
}

// --- UIPanel -----------------------------------------------------------------
UIPanel::UIPanel(std::string name, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> background)
	: UIElement(name, position, size, background) {
}
