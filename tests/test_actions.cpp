#include "catch.hpp"
#include "GameObject.hpp"
#include "Animation.hpp"
#include "Texture.hpp"

TEST_CASE("stopCurrentAction cancels in-progress movement without finishing it", "[actions]") {
    auto obj = std::make_shared<GameObject>("stop_move_obj", Vector2<>(0, 0), Vector2<>(10, 10));
    obj->moveTo(Vector2<>(100, 100), 10.0f);
    obj->stopCurrentAction();

    obj->update(1.0f); // would advance further along the path if not cancelled
    REQUIRE(obj->getPosition().x == 0);
    REQUIRE(obj->getPosition().y == 0);
}

TEST_CASE("stopCurrentAction immediately dismisses displayed text", "[actions]") {
    auto obj = std::make_shared<GameObject>("stop_say_obj", Vector2<>(0, 0), Vector2<>(10, 10));
    obj->say("Hello there", 5.0f);
    REQUIRE(obj->getTextOver().toString() == "Hello there");

    obj->stopCurrentAction();
    REQUIRE(obj->getTextOver().toString().empty());
}

TEST_CASE("stopCurrentAction cancels a one-shot animation, reverting to what played before it", "[actions]") {
    auto obj = std::make_shared<GameObject>("stop_anim_obj", Vector2<>(0, 0), Vector2<>(10, 10));

    auto idle = std::make_shared<Animation>("idle", std::vector<Texture>{ Texture() }, true, 10);
    auto action = std::make_shared<Animation>("action", std::vector<Texture>{ Texture() }, false, 10);
    obj->addAnimation(idle);
    obj->addAnimation(action);
    obj->setAnimation("idle");
    REQUIRE(obj->getAnimation() == idle);

    obj->playAnimation("action");
    REQUIRE(obj->getAnimation() == action);

    obj->stopCurrentAction();
    REQUIRE(obj->getAnimation() == idle);
}

TEST_CASE("stopCurrentAction with nothing in progress is a safe no-op", "[actions]") {
    auto obj = std::make_shared<GameObject>("stop_noop_obj", Vector2<>(5, 5), Vector2<>(10, 10));
    REQUIRE_NOTHROW(obj->stopCurrentAction());
    REQUIRE(obj->getPosition().x == 5);
}

TEST_CASE("say() starts the talk animation and it reverts to idle when the text ends", "[actions][talk]") {
    auto obj = std::make_shared<GameObject>("talk_obj", Vector2<>(0, 0), Vector2<>(10, 10));
    auto idleRight = std::make_shared<Animation>("idleRight", std::vector<Texture>{ Texture() }, true, 10);
    auto idleLeft  = std::make_shared<Animation>("idleLeft",  std::vector<Texture>{ Texture() }, true, 10);
    auto talkRight = std::make_shared<Animation>("talkRight", std::vector<Texture>{ Texture() }, true, 10);
    auto talkLeft  = std::make_shared<Animation>("talkLeft",  std::vector<Texture>{ Texture() }, true, 10);

    obj->setMovementAnimations(nullptr, nullptr, nullptr, nullptr, idleRight, idleLeft, nullptr, nullptr);
    obj->setTalkAnimations(talkRight, talkLeft, nullptr, nullptr);

    // A short timed move to the right establishes the facing direction (and,
    // once it completes, the idle animation) without needing move animations.
    obj->moveTo(Vector2<>(10, 0), 0.1f);
    obj->update(0.2f);
    REQUIRE(obj->getAnimation() == idleRight);
    REQUIRE_FALSE(obj->isTalking());

    obj->say("Hi", 1.0f);
    REQUIRE(obj->isTalking());
    REQUIRE(obj->getAnimation() == talkRight);

    obj->update(1.1f); // say() timer expires
    REQUIRE_FALSE(obj->isTalking());
    REQUIRE(obj->getAnimation() == idleRight);
}

TEST_CASE("say() without talk animations configured leaves the current animation untouched", "[actions][talk]") {
    auto obj = std::make_shared<GameObject>("talk_noop_obj", Vector2<>(0, 0), Vector2<>(10, 10));
    auto idle = std::make_shared<Animation>("idle", std::vector<Texture>{ Texture() }, true, 10);
    obj->addAnimation(idle);
    obj->setAnimation("idle");

    obj->say("Hi", 1.0f);
    REQUIRE_FALSE(obj->isTalking());
    REQUIRE(obj->getAnimation() == idle); // no talk animation was ever configured
}

TEST_CASE("stopCurrentAction stops a mid-speech talk animation too", "[actions][talk]") {
    auto obj = std::make_shared<GameObject>("talk_stop_obj", Vector2<>(0, 0), Vector2<>(10, 10));
    auto idleRight = std::make_shared<Animation>("idleRight2", std::vector<Texture>{ Texture() }, true, 10);
    auto idleLeft  = std::make_shared<Animation>("idleLeft2",  std::vector<Texture>{ Texture() }, true, 10);
    auto talkRight = std::make_shared<Animation>("talkRight2", std::vector<Texture>{ Texture() }, true, 10);
    auto talkLeft  = std::make_shared<Animation>("talkLeft2",  std::vector<Texture>{ Texture() }, true, 10);

    obj->setMovementAnimations(nullptr, nullptr, nullptr, nullptr, idleRight, idleLeft, nullptr, nullptr);
    obj->setTalkAnimations(talkRight, talkLeft, nullptr, nullptr);

    obj->moveTo(Vector2<>(10, 0), 0.1f);
    obj->update(0.2f);

    obj->say("Long speech", 10.0f);
    REQUIRE(obj->isTalking());

    obj->stopCurrentAction();
    REQUIRE_FALSE(obj->isTalking());
    REQUIRE(obj->getAnimation() == idleRight);
    REQUIRE(obj->getTextOver().toString().empty());
}
