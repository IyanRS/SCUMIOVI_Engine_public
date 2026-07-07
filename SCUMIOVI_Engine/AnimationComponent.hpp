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

	std::shared_ptr<Animation> talkUpAnimation = nullptr;
	std::shared_ptr<Animation> talkDownAnimation = nullptr;
	std::shared_ptr<Animation> talkLeftAnimation = nullptr;
	std::shared_ptr<Animation> talkRightAnimation = nullptr;
	bool talking = false;

	bool horizontalAnimationsDefined();
	bool verticalAnimationsDefined();
	bool horizontalIdleDefined();
	bool verticalIdleDefined();
	bool horizontalTalkDefined();
	bool verticalTalkDefined();

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

	// Directional animations shown while the owner is "talking" (see
	// GameObject::say / TextComponent), started/stopped automatically for the
	// duration of the displayed text. Any direction left null simply isn't used.
	void setTalkAnimations(
		std::shared_ptr<Animation> talkRightAnimation, std::shared_ptr<Animation> talkLeftAnimation,
		std::shared_ptr<Animation> talkUpAnimation,    std::shared_ptr<Animation> talkDownAnimation
	);
	// Switches to the talk animation matching the current facing direction; a
	// no-op if no talk animations were set (existing games without them are
	// unaffected). Called by TextComponent when a say() begins.
	void startTalking();
	// Reverts to the idle animation for the current facing direction. Called
	// by TextComponent when the displayed text ends (naturally or cancelled).
	void stopTalking();
	bool isTalking() const;

	// Cancels a one-shot animation (from playAnimation) in progress, jumping
	// straight to its return-to animation without firing the completion
	// callback. A no-op if no one-shot animation is playing.
	void cancelOneShot();
};
