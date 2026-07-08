#include "catch.hpp"
#include "Game.hpp"
#include "Event.hpp"
#include "Player.hpp"

static void drainPendingEvents() {
    for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

TEST_CASE("Fade starts inactive", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    REQUIRE(game.getRenderer().isFading() == false);
}

TEST_CASE("FadeToBlack activates and completes", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    r.fadeToBlack(1.0f);
    REQUIRE(r.isFading() == true);

    r.updateFade(0.5f);
    REQUIRE(r.isFading() == true);

    r.updateFade(0.6f);
    REQUIRE(r.isFading() == false);
}

TEST_CASE("FadeFromBlack activates and completes", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    r.fadeFromBlack(0.5f);
    REQUIRE(r.isFading() == true);

    r.updateFade(0.3f);
    REQUIRE(r.isFading() == true);

    r.updateFade(0.3f);
    REQUIRE(r.isFading() == false);
}

TEST_CASE("Fade to black fires callback on completion", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    bool fired = false;
    r.fadeToBlack(0.5f, Event([&fired]() { fired = true; }));
    REQUIRE(fired == false);

    r.updateFade(0.6f);
    REQUIRE(fired == true);
}

TEST_CASE("Fade from black fires callback on completion", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    bool fired = false;
    r.fadeFromBlack(0.5f, Event([&fired]() { fired = true; }));
    REQUIRE(fired == false);

    r.updateFade(0.6f);
    REQUIRE(fired == true);
}

TEST_CASE("Fade with zero duration completes immediately", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    bool fired = false;
    r.fadeToBlack(0.0f, Event([&fired]() { fired = true; }));
    r.updateFade(0.016f);
    REQUIRE(r.isFading() == false);
    REQUIRE(fired == true);
}

TEST_CASE("changeScene with no fade completes immediately", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();

    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();
    game.changeScene(sceneA);
    REQUIRE(game.getScene() == sceneA);

    game.changeScene(sceneB, 0.0f);
    REQUIRE(game.getScene() == sceneB);
}

TEST_CASE("changeScene with fade delays the scene swap", "[fade]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();
    game.changeScene(sceneA);
    REQUIRE(game.getScene() == sceneA);

    game.changeScene(sceneB, 0.5f);
    // Scene should not swap yet — still fading to black
    REQUIRE(game.getScene() == sceneA);
    REQUIRE(r.isFading() == true);

    // Complete the fade
    for (int i = 0; i < 100; i++) r.updateFade(0.1f);
    REQUIRE(r.isFading() == false);
    // Scene should now be B
    REQUIRE(game.getScene() == sceneB);
}

TEST_CASE("a fade-completion callback that starts a second fade keeps its own callback", "[fade]") {
    // Regression test: updateFade used to clear fadeCompleteEvent *after*
    // executing it, which silently wiped out a new callback set by the
    // callback itself (e.g. Game::changeScene's fade-to-black completion
    // starting the fade-from-black with the caller's real continuation).
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    // Event::execute() (the no-EventInfo overload fadeCompleteEvent uses) is a
    // no-op without a current player; set one explicitly rather than relying
    // on whatever an earlier, unrelated test left behind.
    auto player = std::make_shared<Player>("fade_regression_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    bool innerFired = false;
    r.fadeToBlack(0.3f, Event([&r, &innerFired]() {
        r.fadeFromBlack(0.3f, Event([&innerFired]() { innerFired = true; }));
    }));

    for (int i = 0; i < 4; i++) r.updateFade(0.1f); // ~0.4s: completes the 0.3s fade-to-black, starts fade-from-black
    REQUIRE_FALSE(innerFired);                      // now mid fade-from-black, not done yet
    REQUIRE(r.isFading());

    for (int i = 0; i < 4; i++) r.updateFade(0.1f); // another ~0.4s: completes the fade-from-black
    REQUIRE(innerFired);
    REQUIRE_FALSE(r.isFading());
}
