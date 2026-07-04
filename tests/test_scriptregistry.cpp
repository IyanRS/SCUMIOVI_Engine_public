#include "catch.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Event.hpp"
#include "EventInfo.hpp"

TEST_CASE("Script registry registers and runs an EventInfo script", "[script]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("script_player_1", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int ran = 0;
    game.registerScript("incr", [&ran](EventInfo e) { ran++; });

    REQUIRE(game.hasScript("incr"));
    REQUIRE_FALSE(game.hasScript("not_registered"));

    game.scriptEvent("incr").execute();
    REQUIRE(ran == 1);
}

TEST_CASE("Script registry supports the void() overload", "[script]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("script_player_2", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int ran = 0;
    game.registerScript("void_script", [&ran]() { ran++; });
    game.scriptEvent("void_script").execute();
    REQUIRE(ran == 1);
}

TEST_CASE("Script registry re-registration replaces the script", "[script]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("script_player_3", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int a = 0, b = 0;
    game.registerScript("dup", [&a]() { a++; });
    game.registerScript("dup", [&b]() { b++; });
    game.scriptEvent("dup").execute();
    REQUIRE(a == 0);
    REQUIRE(b == 1);
}

TEST_CASE("scriptEvent for a missing script is a safe no-op", "[script]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("script_player_4", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    REQUIRE_NOTHROW(game.scriptEvent("definitely_missing").execute());
}

TEST_CASE("scriptEvent is late-bound (registration may follow event creation)", "[script]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("script_player_5", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int ran = 0;
    Event ev = game.scriptEvent("late");        // created before registration
    game.registerScript("late", [&ran]() { ran++; });
    ev.execute();
    REQUIRE(ran == 1);
}
