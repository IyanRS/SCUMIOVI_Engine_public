#pragma once

#include "Vector2.hpp"
#include "Animation.hpp"
#include "Event.hpp"
#include "Interactable.hpp"
#include "MovementComponent.hpp"
#include "TextComponent.hpp"
#include "AnimationComponent.hpp"
#include "GraphicsComponent.hpp"

class Scene;

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

#include "Text.hpp"
#include "Dialogue.hpp"

enum VerticalDirection { UP, DOWN };
enum HorizontalDirection { LEFT, RIGHT };

class GameObject : public Interactable, public std::enable_shared_from_this<GameObject> {
	friend class MovementComponent;
	friend class TextComponent;
	friend class AnimationComponent;
	friend class GraphicsComponent;

	std::string name;
	Vector2<> position;
	Vector2<> size;
	bool interactable = true;
	std::shared_ptr<Scene> currentScene = nullptr;
	std::map<std::shared_ptr<GameObject>, Event> overlapEvents;
	MovementComponent movement;
	TextComponent text;
	AnimationComponent animator;
	GraphicsComponent graphics;

	int state = 0;
	std::string examineText;

	void setIdleAnimation(Vector2<> lastMovementDirection);
	void setMovementAnimation(Vector2<> movementDirection);
	bool horizontalAnimationsDefined();
	bool verticalAnimationsDefined();
	bool horizontalIdleDefined();
	bool verticalIdleDefined();

	std::unordered_set<std::string> objectTags;

public:
	GameObject(std::string name,
			   Vector2<> position = Vector2<>(), 
			   Vector2<> size = Vector2<>(-1,-1));
	GameObject(std::string name,
		Vector2<> position,
		Vector2<> size, 
		std::shared_ptr<Texture> texture);
	GameObject(std::string name,
		Vector2<> position,
		Vector2<> size,
		std::shared_ptr<Animation> animation);
	Vector2<> getPosition();
	void setPosition(Vector2<> position);
	Vector2<> getSize();
	void setSize(Vector2<> size);
	std::shared_ptr<Texture> getTexture();
	void setTexture(std::string textureName);
	void addTexture(std::shared_ptr<Texture> texture);
	std::shared_ptr<Animation> getAnimation();
	void addAnimation(std::shared_ptr<Animation> animation);
	void setAnimation(std::string animationName);
	void playAnimation(std::string animationName, Event event = Event());
	std::shared_ptr<Animation> getAnimationByName(std::string name);
	void addOverlapEvent(std::shared_ptr<GameObject> object, Event event);
	std::map<std::shared_ptr<GameObject>, Event>& getOverlapEvents();
	Vector2<> getDirectionVector();
	void moveToScene(std::shared_ptr<Scene> scene);
	std::shared_ptr<Scene> getScene();
	void setScene(std::shared_ptr<Scene> scene);
	void moveTo(Vector2<> pos, float seconds = 0, Event callback = Event());
	void moveToAtSpeed(Vector2<> pos, float speed, Event callback = Event());
	void moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> = std::vector<Event>());
	void toggleVisibility();
	void setVisible(bool visible);
	void destroy();
	bool isVisible();
	std::string& getName();
	bool operator==(const GameObject& p2) const;
	bool operator!=(const GameObject& p2) const;
	void setLayer(int layer);
	int getLayer();
	Text& getTextOver();
	void setTextOver(std::string textOver);
	void setTextOver(Text& textOver);
	void say(Text& textOver, float seconds = 0, Event callback = Event());
	void say(std::string text, float seconds = 0, Event callback = Event());
	void setState(int state);
	int getState();
	void setExamineText(const std::string& text);
	const std::string& getExamineText() const;
	void setTextFormat(Text& text);
	void addTag(std::string tag);
	bool hasTag(std::string tag);
	VerticalDirection getCurrentVerticalDirection();
	HorizontalDirection getCurrentHorizontalDirection();
	void setMovementAnimations(std::shared_ptr<Animation> moveRightAnimation, std::shared_ptr<Animation> moveLeftAnimation,
		std::shared_ptr<Animation> moveUpAnimation, std::shared_ptr<Animation> moveDownAnimation,
		std::shared_ptr<Animation> idleRightAnimation, std::shared_ptr<Animation> idleLeftAnimation,
		std::shared_ptr<Animation> idleUpAnimation, std::shared_ptr<Animation> idleDownAnimation);
	void setTalkAnimations(std::shared_ptr<Animation> talkRightAnimation, std::shared_ptr<Animation> talkLeftAnimation,
		std::shared_ptr<Animation> talkUpAnimation, std::shared_ptr<Animation> talkDownAnimation);
	bool isTalking();

	// Cancels any in-progress movement, one-shot animation and displayed
	// "say" text immediately (no completion callbacks fire). Used when
	// loading a save so a mid-action object doesn't keep executing an action
	// that's no longer relevant after being repositioned.
	void stopCurrentAction();

	// Called once per frame by Scene::update()
	void update(float deltaTime);
};