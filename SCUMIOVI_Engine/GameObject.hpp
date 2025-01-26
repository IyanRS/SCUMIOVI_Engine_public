#pragma once

#include "Vector2.hpp"
#include "Animation.hpp"
#include "Event.hpp"
#include "Interactable.hpp"

class Scene;
//#include "Scene.hpp"

#include <list>
#include <map>
#include <string>
#include <vector>

#include <string>
#include "Text.hpp"

class GameObject : public Interactable {
	std::string name;
	Vector2<> position;
	Animation* animation;
	std::vector<Animation*> animations;
	std::vector<Texture*> textures;
	Texture* texture;
	int layer = 0;
	Vector2<> size;
	bool interactable = true;
	//Event clickEvent;
	Scene* currentScene;
	std::map<GameObject*, Event> overlapEvents;
	Text textOver;
	Text textFormat;
	Vector2<> currentMovingTarget = Vector2<>(-INFINITY, -INFINITY);
	Vector2<> lastMovementDirection = Vector2<>(0, 0);
	void moveToTask(Vector2<> position, float seconds = 0, Event callback = Event());
	void sayTask(Text& textOver, float seconds, Event callback = Event());
	bool moving = false;
	bool visibility = true;
	int state;
	int dialogDefaultTextColor[4];

	void setIdleAnimation(Vector2<> lastMovementDirection);
	void setMovementAnimation(Vector2<> movementDirection);
	bool horizontalAnimationsDefined();
	bool verticalAnimationsDefined();
	bool horizontalIdleDefined();
	bool verticalIdleDefined();
	//Animation* moveUpAnimation = nullptr;
	//Animation* moveDownAnimation = nullptr;
	//Animation* moveLeftAnimation = nullptr;
	//Animation* moveRightAnimation = nullptr;
	//Animation* idleUpAnimation = nullptr;
	//Animation* idleDownAnimation = nullptr;
	//Animation* idleLeftAnimation = nullptr;
	//Animation* idleRightAnimation = nullptr;
	
public:
	GameObject(std::string name,
			   Vector2<> position = Vector2<>(), 
			   Vector2<> size = Vector2<>(-1,-1));
	GameObject(std::string name,
		Vector2<> position,
		Vector2<> size, 
		Texture* texture);
	GameObject(std::string name,
		Vector2<> position,
		Vector2<> size,
		Animation* animation);
	Vector2<> getPosition();
	void setPosition(Vector2<> position);
	Vector2<> getSize();
	void setSize(Vector2<> size);
	Texture* getTexture();
	void setTexture(std::string textureName);
	void addTexture(Texture* texture);
	Animation* getAnimation();
	void addAnimation(Animation* animation);
	void setAnimation(std::string animatioName);
	Animation* getAnimationByName(std::string name);
	//void setAnimation(Animation& animation);
	//Event& getClickEvent();
	void addOverlapEvent(GameObject* object, Event event);
	std::map<GameObject*, Event>& getOverlapEvents();
	Vector2<> getDirectionVector();
	void moveToScene(Scene& scene);
	void setScene(Scene* scene);
	void moveTo(Vector2<> pos, float seconds = 0, Event callback = Event());
	void moveToAtSpeed(Vector2<> pos, float speed, Event callback = Event());
	void moveToAtSpeed(std::vector<Vector2<>> positions, float speed, std::vector<Event> = std::vector<Event>());
	void toggleVisibility();
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
	void setTextFormat(Text& text);
	//void setMovementAnimations(Animation* moveUp, Animation* moveDown, Animation* moveLeft, Animation* moveRight);
	//void setIdleAnimations(Animation* idleUp, Animation* idleDown, Animation* idleLeft, Animation* idleRight);
};