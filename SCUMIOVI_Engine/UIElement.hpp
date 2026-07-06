#pragma once

#include "GameObject.hpp"
#include "Text.hpp"
#include "Event.hpp"
#include "Texture.hpp"

#include <memory>
#include <string>
#include <vector>

// Nine-point anchor used to position an element relative to its parent (or the
// screen for a top-level element); see UILayout's flatten pass for how the
// anchor and offset combine into an absolute position.
enum class UIAnchor {
	TopLeft, TopCenter, TopRight,
	CenterLeft, Center, CenterRight,
	BottomLeft, BottomCenter, BottomRight
};

// -----------------------------------------------------------------------------
// UIElement
//
// Base class for the data-driven UI. A UIElement is a GameObject (so it renders,
// carries text, participates in layer ordering, and handles clicks through
// Interactable) plus a tree of child elements and a parent-relative offset.
//
// Layouts flatten this tree into an absolute-positioned draw list (see
// UILayout::getRenderElements), so the renderer and input code stay unchanged.
// -----------------------------------------------------------------------------
class UIElement : public GameObject {
	// Children are plain GameObjects: most are UIElements (recursed into for
	// both flattening and refresh), but leaf content like inventory item icons
	// is an ordinary GameObject placed directly by a widget's refresh().
	std::vector<std::shared_ptr<GameObject>> children;
	Vector2<> offset;                 // position relative to the parent element
	UIAnchor anchor = UIAnchor::TopLeft;

public:
	UIElement(std::string name, Vector2<> position = Vector2<>(), Vector2<> size = Vector2<>(-1, -1));
	UIElement(std::string name, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> texture);
	virtual ~UIElement() = default;

	void addChild(std::shared_ptr<GameObject> child);
	std::vector<std::shared_ptr<GameObject>>& getChildren();

	Vector2<> getOffset() const;
	void setOffset(Vector2<> newOffset);
	UIAnchor getAnchor() const;
	void setAnchor(UIAnchor newAnchor);

	// Per-frame hook to refresh the element from game state. The default walks
	// the children; widget subclasses override it (inventory, verb info, ...).
	virtual void refresh();
};

// --- Generic element types ---------------------------------------------------
// Small, cohesive concrete elements kept together with the base to avoid
// scattering four tiny classes (and their build entries) across the tree.

// A static picture (or solid colour, via a colour Texture).
class UIImage : public UIElement {
public:
	UIImage(std::string name, Vector2<> position, Vector2<> size, std::shared_ptr<Texture> texture);
};

// A piece of text; size defaults to zero because text renders from its Text.
class UILabel : public UIElement {
	// Source-locale text, set via setLocalizedText(); empty if the label was
	// given a fixed Text directly and should not be re-resolved on refresh.
	std::string sourceText;
public:
	UILabel(std::string name, Vector2<> position, Text text);
	// Marks this label's text as translatable: it is resolved through tr()
	// immediately and again on every refresh(), so a locale switch while the
	// label is visible updates it without rebuilding the element.
	void setLocalizedText(std::string text);
	void refresh() override;
};

// A clickable element. onClick runs on any click (bound through the engine's
// "any" verb); onHover runs on mouse-over. Both are ordinary Events, so they can
// be a scriptEvent(name) from data.
class UIButton : public UIElement {
	std::string sourceText;
public:
	UIButton(std::string name, Vector2<> position, Vector2<> size,
		std::shared_ptr<Texture> texture, Text label,
		Event onClick, Event onHover = Event());
	// See UILabel::setLocalizedText.
	void setLocalizedText(std::string text);
	void refresh() override;
};

// A container with an optional background; children are positioned relative to it.
class UIPanel : public UIElement {
public:
	UIPanel(std::string name, Vector2<> position, Vector2<> size,
		std::shared_ptr<Texture> background = nullptr);
};
