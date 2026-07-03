#include "catch.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "GameObject.hpp"
#include "Verb.hpp"
#include "Event.hpp"
#include "EventInfo.hpp"

// ---------------------------------------------------------------------------
// Helper: drain leftover pending events from previous sections
// ---------------------------------------------------------------------------
static void drainPendingEvents() {
    for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

TEST_CASE("Examine text API", "[examine]") {
    drainPendingEvents();

    SECTION("examineText defaults to empty") {
        GameObject obj("box", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE(obj.getExamineText().empty());
    }

    SECTION("setExamineText and getExamineText round-trip") {
        GameObject obj("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj.setExamineText("It's a sturdy cardboard box.");
        REQUIRE(obj.getExamineText() == "It's a sturdy cardboard box.");
    }

    SECTION("setExamineText overwrites previous text") {
        GameObject obj("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj.setExamineText("First description.");
        obj.setExamineText("Second description.");
        REQUIRE(obj.getExamineText() == "Second description.");
    }
}

TEST_CASE("Examine text default event integration", "[examine][gameplay]") {
    drainPendingEvents();

    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    SECTION("lookAt on object with examineText does not crash") {
        auto obj = std::make_shared<GameObject>("rock", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->setExamineText("It's a rock.");
        REQUIRE_NOTHROW(obj->interact(Vector2<>(5, 5), game.lookAtVerb));
    }

    SECTION("lookAt on object without examineText uses fallback") {
        auto obj = std::make_shared<GameObject>("rock", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE_NOTHROW(obj->interact(Vector2<>(5, 5), game.lookAtVerb));
    }

    SECTION("examineText does not affect other verbs") {
        auto obj = std::make_shared<GameObject>("wall", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->setExamineText("A solid wall.");
        bool pushFired = false;
        obj->addVerbInteraction(game.pushVerb, Event([&pushFired](EventInfo) { pushFired = true; }));
        obj->interact(Vector2<>(5, 5), game.pushVerb);
        REQUIRE(pushFired == true);
    }
}
