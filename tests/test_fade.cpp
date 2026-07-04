#include "catch.hpp"
#include "Game.hpp"
#include "Event.hpp"

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
