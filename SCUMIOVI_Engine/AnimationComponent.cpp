#include "AnimationComponent.hpp"
#include "GameObject.hpp"
#include "Animation.hpp"

AnimationComponent::AnimationComponent(GameObject* owner) : owner(owner) {}

void AnimationComponent::update(float deltaTime) {
	if (animReturnFrame >= 0 && animation != nullptr) {
		if (animation->currentFrame >= animReturnFrame - 1) {
			animation       = animReturnTo;
			animReturnTo    = nullptr;
			animReturnFrame = -1;
			animCallback.execute();
			animCallback    = Event();
		}
	}
}

Animation* AnimationComponent::getAnimation() {
	return animation;
}

void AnimationComponent::addAnimation(Animation* anim) {
	animations.push_back(anim);
}

Animation* AnimationComponent::getAnimationByName(std::string name) {
	if (name == "")
		return nullptr;
	for (Animation* anim : animations) {
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
	for (Animation* anim : animations) {
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
	for (Animation* anim : animations) {
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
	Animation* moveRight, Animation* moveLeft,
	Animation* moveUp,    Animation* moveDown,
	Animation* idleRight, Animation* idleLeft,
	Animation* idleUp,    Animation* idleDown)
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

	if ((abs(movementDirection.x) > abs(movementDirection.y)
		|| !verticalAnimationsDefined()) && horizontalAnimationsDefined()) {
		animation = (movementDirection.x > 0) ? moveRightAnimation : moveLeftAnimation;
	} else if (verticalAnimationsDefined()) {
		animation = (movementDirection.y > 0) ? moveDownAnimation : moveUpAnimation;
	}
}
