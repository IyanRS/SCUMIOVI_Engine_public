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
	Event mouseOverEvent;
	//Event clickEvent;
	Scene* currentScene;
	std::map<GameObject*, Event> overlapEvents;
	std::string textOver;
	Vector2<> currentMovingTarget = Vector2<>(-INFINITY, -INFINITY);
	void moveToTask(Vector2<> position, float seconds = 0, Event callback = Event());
	bool moving = false;
	bool visibility = true;
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
	//Animation* getAnimationByName(std::string name);
	//void setAnimation(Animation& animation);
	Event& getMouseOverEvent();
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
	std::string getTextOver();
	void setTextOver(std::string textOver);
};