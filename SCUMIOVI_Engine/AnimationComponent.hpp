#pragma once

#include "Vector2.hpp"
#include "Event.hpp"
#include <vector>
#include <string>

class Animation;
class GameObject;

class AnimationComponent {
	friend class GameObject;
private:
	GameObject* owner;

	Animation* animation = nullptr;
	std::vector<Animation*> animations;

	Animation* animReturnTo  = nullptr;
	int        animReturnFrame = -1;
	Event      animCallback;

	Animation* moveUpAnimation = nullptr;
	Animation* moveDownAnimation = nullptr;
	Animation* moveLeftAnimation = nullptr;
	Animation* moveRightAnimation = nullptr;
	Animation* idleUpAnimation = nullptr;
	Animation* idleDownAnimation = nullptr;
	Animation* idleLeftAnimation = nullptr;
	Animation* idleRightAnimation = nullptr;

	bool horizontalAnimationsDefined();
	bool verticalAnimationsDefined();
	bool horizontalIdleDefined();
	bool verticalIdleDefined();

public:
	AnimationComponent(GameObject* owner);

	void update(float deltaTime);

	Animation* getAnimation();
	void addAnimation(Animation* anim);
	void setAnimation(std::string animationName);
	Animation* getAnimationByName(std::string name);
	void playAnimation(std::string animationName, Event event = Event());

	void setIdleAnimation(Vector2<> lastMovementDirection);
	void setMovementAnimation(Vector2<> movementDirection);

	void setMovementAnimations(
		Animation* moveRightAnimation, Animation* moveLeftAnimation,
		Animation* moveUpAnimation,    Animation* moveDownAnimation,
		Animation* idleRightAnimation, Animation* idleLeftAnimation,
		Animation* idleUpAnimation,    Animation* idleDownAnimation
	);
};
