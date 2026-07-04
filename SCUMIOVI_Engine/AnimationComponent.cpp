#include "AnimationComponent.hpp"
#include "GameObject.hpp"
#include "Animation.hpp"
#include <memory>
#include <cmath>

AnimationComponent::AnimationComponent(GameObject* owner) : owner(owner) {}

void AnimationComponent::update(float deltaTime) {
	// Return to the previous animation once a one-shot animation has finished.
	// Checked before advancing so it reads the prior frame's counter, matching
	// the ordering from when the renderer performed the advance.
	if (animation != nullptr && animReturnTo != nullptr && animReturnFrame >= 0) {
		if (animation->currentFrame >= animReturnFrame - 1) {
			animation       = animReturnTo;
			animReturnTo    = nullptr;
			animReturnFrame = -1;
			animCallback.execute();
			animCallback    = Event();
		}
	}

	// Advance the current animation's frame counter (previously done inside the
	// renderer). Looping animations wrap; one-shot animations hold on the last
	// frame until the return-to logic above swaps them out.
	if (animation != nullptr) {
		int totalTicks = (int)animation->getFrames().size() * animation->getFrameRate();
		if (animation->currentFrame < totalTicks - 1) {
			animation->currentFrame++;
		}
		else if (animation->doesLoop()) {
			animation->currentFrame = 0;
		}
	}
}

std::shared_ptr<Animation> AnimationComponent::getAnimation() {
	return animation;
}

void AnimationComponent::addAnimation(std::shared_ptr<Animation> anim) {
	animations.push_back(anim);
}

std::shared_ptr<Animation> AnimationComponent::getAnimationByName(std::string name) {
	if (name == "")
		return nullptr;
	for (auto anim : animations) {
		if (anim->getName() == name) {
			return anim;
		}
	}
	return nullptr;
}

void AnimationComponent::setAnimation(std::string animationName) {
	if (animationName == "") {
		animation = nullptr;
		return;
	}
	for (auto anim : animations) {
		if (anim->getName() == animationName) {
			animation = anim;
		}
	}
}

void AnimationComponent::playAnimation(std::string animationName, Event callback) {
	if (animationName == "") {
		animation = nullptr;
		return;
	}
	for (auto anim : animations) {
		if (anim->getName() == animationName) {
			if (!anim->doesLoop()) {
				animReturnTo    = animation;
				anim->currentFrame = 0;
				animation       = anim;
				animReturnFrame = (int)anim->getFrames().size() * anim->getFrameRate();
				animCallback    = callback;
			} else {
				setAnimation(anim->getName());
			}
			return;
		}
	}
}

bool AnimationComponent::horizontalAnimationsDefined() {
	return moveRightAnimation != nullptr && moveLeftAnimation != nullptr;
}

bool AnimationComponent::verticalAnimationsDefined() {
	return moveUpAnimation != nullptr && moveDownAnimation != nullptr;
}

bool AnimationComponent::horizontalIdleDefined() {
	return idleRightAnimation != nullptr && idleLeftAnimation != nullptr;
}

bool AnimationComponent::verticalIdleDefined() {
	return idleUpAnimation != nullptr && idleDownAnimation != nullptr;
}

void AnimationComponent::setIdleAnimation(Vector2<> lastDir) {
	if ((abs(lastDir.x) > abs(lastDir.y) || !verticalIdleDefined()) && horizontalIdleDefined()) {
		animation = (lastDir.x > 0) ? idleRightAnimation : idleLeftAnimation;
	} else if (verticalIdleDefined()) {
		animation = (lastDir.y > 0) ? idleUpAnimation : idleDownAnimation;
	}
}

void AnimationComponent::setMovementAnimations(
	std::shared_ptr<Animation> moveRight, std::shared_ptr<Animation> moveLeft,
	std::shared_ptr<Animation> moveUp,    std::shared_ptr<Animation> moveDown,
	std::shared_ptr<Animation> idleRight, std::shared_ptr<Animation> idleLeft,
	std::shared_ptr<Animation> idleUp,    std::shared_ptr<Animation> idleDown)
{
	moveRightAnimation = moveRight;
	moveLeftAnimation  = moveLeft;
	moveUpAnimation    = moveUp;
	moveDownAnimation  = moveDown;
	idleRightAnimation = idleRight;
	idleLeftAnimation  = idleLeft;
	idleUpAnimation    = idleUp;
	idleDownAnimation  = idleDown;
}

void AnimationComponent::setMovementAnimation(Vector2<> movementDirection) {
	if (movementDirection.x == 0 && movementDirection.y == 0) {
		setIdleAnimation(owner->movement.getLastMovementDirection());
		return;
	}

	owner->movement.setLastMovementDirection(movementDirection);

	if ((std::abs(movementDirection.x) > std::abs(movementDirection.y)
		|| !verticalAnimationsDefined()) && horizontalAnimationsDefined()) {
		animation = (movementDirection.x > 0) ? moveRightAnimation : moveLeftAnimation;
	} else if (verticalAnimationsDefined()) {
		animation = (movementDirection.y > 0) ? moveDownAnimation : moveUpAnimation;
	}
}
