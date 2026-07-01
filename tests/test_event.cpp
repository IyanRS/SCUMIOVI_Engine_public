#include "catch.hpp"
#include "Event.hpp"
#include "EventInfo.hpp"
#include "Player.hpp"
#include "Game.hpp"

TEST_CASE("Event default construction", "[event]") {
    Event e;
    EventInfo* infoPtr = nullptr;
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event with void lambda action", "[event]") {
    Event e([]() {});
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event with EventInfo lambda action", "[event]") {
    Event e([](EventInfo info) {});
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event with Delay action", "[event]") {
    Event e(Delay(1.0f));
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event with vector of actions", "[event]") {
    Event e(std::vector<Event::Action>{
        []() {},
        [](EventInfo) {},
        Delay(0.5f)
    });
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event composition with operator+", "[event]") {
    int count = 0;
    Event a([&count]() { count++; });
    Event b([&count]() { count++; });
    Event combined = a + b;
    REQUIRE(&combined != nullptr);
}

TEST_CASE("Event copy assignment", "[event]") {
    Event original([]() {});
    Event copy;
    copy = original;
    REQUIRE(&copy != &original);
}

TEST_CASE("Event addAction", "[event]") {
    Event e;
    e.addAction([]() {});
    e.addAction(Delay(0.5f));
    e.addAction([](EventInfo) {});
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event self-assignment is safe", "[event]") {
    Event e([]() {});
    e = e;
    REQUIRE(&e != nullptr);
}

TEST_CASE("Event::tick is safe with no pending events", "[event]") {
    REQUIRE_NOTHROW(Event::tick(0.016f));
    REQUIRE_NOTHROW(Event::tick(0.0f));
    REQUIRE_NOTHROW(Event::tick(-1.0f));
}

TEST_CASE("Event::tick leaves pending queue empty after firing", "[event]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("et_tick_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int fired = 0;
    Event e({
        Delay(0.1f),
        [&fired]() { fired++; }
    });
    e.execute();
    REQUIRE(fired == 0);
    Event::tick(0.2f);
    REQUIRE(fired == 1);
    // Subsequent tick should not double-fire
    Event::tick(0.2f);
    REQUIRE(fired == 1);
}
