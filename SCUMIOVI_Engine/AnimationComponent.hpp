#pragma once

#include "Vector2.hpp"
#include "Event.hpp"
#include <vector>
#include <string>
#include <memory>

class Animation;
class GameObject;

class AnimationComponent {
	friend class GameObject;
private:
	GameObject* owner;

	std::shared_ptr<Animation> animation = nullptr;
	std::vector<std::shared_ptr<Animation>> animations;

	std::shared_ptr<Animation> animReturnTo = nullptr;
	int        animReturnFrame = -1;
	Event      animCallback;

	std::shared_ptr<Animation> moveUpAnimation = nullptr;
	std::shared_ptr<Animation> moveDownAnimation = nullptr;
	std::shared_ptr<Animation> moveLeftAnimation = nullptr;
	std::shared_ptr<Animation> moveRightAnimation = nullptr;
	std::shared_ptr<Animation> idleUpAnimation = nullptr;
	std::shared_ptr<Animation> idleDownAnimation = nullptr;
	std::shared_ptr<Animation> idleLeftAnimation = nullptr;
	std::shared_ptr<Animation> idleRightAnimation = nullptr;

	bool horizontalAnimationsDefined();
	bool verticalAnimationsDefined();
	bool horizontalIdleDefined();
	bool verticalIdleDefined();

public:
	AnimationComponent(GameObject* owner);

	void update(float deltaTime);

	std::shared_ptr<Animation> getAnimation();
	void addAnimation(std::shared_ptr<Animation> anim);
	void setAnimation(std::string animationName);
	std::shared_ptr<Animation> getAnimationByName(std::string name);
	void playAnimation(std::string animationName, Event event = Event());

	void setIdleAnimation(Vector2<> lastMovementDirection);
	void setMovementAnimation(Vector2<> movementDirection);

	void setMovementAnimations(
		std::shared_ptr<Animation> moveRightAnimation, std::shared_ptr<Animation> moveLeftAnimation,
		std::shared_ptr<Animation> moveUpAnimation,    std::shared_ptr<Animation> moveDownAnimation,
		std::shared_ptr<Animation> idleRightAnimation, std::shared_ptr<Animation> idleLeftAnimation,
		std::shared_ptr<Animation> idleUpAnimation,    std::shared_ptr<Animation> idleDownAnimation
	);
};
