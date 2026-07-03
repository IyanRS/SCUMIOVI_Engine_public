#include "catch.hpp"
#include "GameState.hpp"

TEST_CASE("GameState default flag is false", "[gamestate]") {
    GameState gs;
    REQUIRE(gs.getFlag("nonexistent") == false);
}

TEST_CASE("GameState setFlag and getFlag", "[gamestate]") {
    GameState gs;
    gs.setFlag("has_key");
    REQUIRE(gs.getFlag("has_key") == true);
}

TEST_CASE("GameState setFlag false explicitly", "[gamestate]") {
    GameState gs;
    gs.setFlag("has_key", false);
    REQUIRE(gs.getFlag("has_key") == false);
}

TEST_CASE("GameState clearFlag removes flag", "[gamestate]") {
    GameState gs;
    gs.setFlag("has_key");
    REQUIRE(gs.getFlag("has_key") == true);
    gs.clearFlag("has_key");
    REQUIRE(gs.getFlag("has_key") == false);
}

TEST_CASE("GameState default counter is 0", "[gamestate]") {
    GameState gs;
    REQUIRE(gs.getCounter("score") == 0);
}

TEST_CASE("GameState setCounter and getCounter", "[gamestate]") {
    GameState gs;
    gs.setCounter("score", 10);
    REQUIRE(gs.getCounter("score") == 10);
}

TEST_CASE("GameState incrementCounter", "[gamestate]") {
    GameState gs;
    gs.setCounter("score", 5);
    gs.incrementCounter("score");
    REQUIRE(gs.getCounter("score") == 6);
}

TEST_CASE("GameState incrementCounter by delta", "[gamestate]") {
    GameState gs;
    gs.setCounter("score", 5);
    gs.incrementCounter("score", 3);
    REQUIRE(gs.getCounter("score") == 8);
}

TEST_CASE("GameState incrementCounter on default 0", "[gamestate]") {
    GameState gs;
    gs.incrementCounter("apples");
    REQUIRE(gs.getCounter("apples") == 1);
}

TEST_CASE("GameState flags and counters are independent", "[gamestate]") {
    GameState gs;
    gs.setFlag("has_key");
    gs.setCounter("score", 42);
    REQUIRE(gs.getFlag("has_key") == true);
    REQUIRE(gs.getCounter("score") == 42);
    REQUIRE(gs.getFlag("score") == false);
    REQUIRE(gs.getCounter("has_key") == 0);
}
