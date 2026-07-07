#include "catch.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Scene.hpp"
#include "UIAction.hpp"

// NOTE: Game is a process-wide singleton reused across every TEST_CASE in this
// binary, so `game.getPlayers()` always contains players added by earlier
// tests too. Tests below only assert relationships between players they
// created themselves in the same test body (relying on addPlayer's insertion
// order being preserved, which is safe since Catch2 runs test cases
// sequentially) rather than assuming the party list is otherwise empty.

TEST_CASE("switchToPlayer switches the current player", "[party]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_basic_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_basic_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);

    game.setCurrentPlayer(a);
    REQUIRE(game.switchToPlayer(b));
    REQUIRE(game.getCurrentPlayer() == b);
}

TEST_CASE("switchToPlayer fails for null or a player never added to the party", "[party]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_reject_a", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.setCurrentPlayer(a);

    REQUIRE_FALSE(game.switchToPlayer(std::shared_ptr<Player>(nullptr)));
    REQUIRE(game.getCurrentPlayer() == a);

    auto stranger = std::make_shared<Player>("party_stranger", Vector2<>(0, 0), Vector2<>(10, 10));
    REQUIRE_FALSE(game.switchToPlayer(stranger));
    REQUIRE(game.getCurrentPlayer() == a);
}

TEST_CASE("switchToPlayer by name works and fails gracefully for an unknown name", "[party]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_byname_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_byname_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);
    game.setCurrentPlayer(a);

    REQUIRE(game.switchToPlayer(std::string("party_byname_b")));
    REQUIRE(game.getCurrentPlayer() == b);

    REQUIRE_FALSE(game.switchToPlayer(std::string("definitely_not_a_party_member")));
    REQUIRE(game.getCurrentPlayer() == b); // unchanged after the failed lookup
}

TEST_CASE("switchToPlayer switches the active scene to match the target player's own scene", "[party]") {
    Game& game = Game::getInstance();
    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();

    auto a = std::make_shared<Player>("party_scene_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_scene_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);
    a->moveToScene(sceneA);
    b->moveToScene(sceneB);

    game.setCurrentPlayer(a);
    game.changeScene(sceneA);
    REQUIRE(game.getScene() == sceneA);

    REQUIRE(game.switchToPlayer(b));
    REQUIRE(game.getCurrentPlayer() == b);
    REQUIRE(game.getScene() == sceneB); // scene follows b into its own room

    REQUIRE(game.switchToPlayer(a));
    REQUIRE(game.getScene() == sceneA);
}

TEST_CASE("switchToPlayer with fade delays the scene swap until the fade completes", "[party]") {
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();
    auto a = std::make_shared<Player>("party_fade_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_fade_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);
    a->moveToScene(sceneA);
    b->moveToScene(sceneB);

    game.setCurrentPlayer(a);
    game.changeScene(sceneA);
    REQUIRE(game.getScene() == sceneA);

    REQUIRE(game.switchToPlayer(b, 0.5f));
    REQUIRE(game.getCurrentPlayer() == a); // not yet — still fading to black
    REQUIRE(game.getScene() == sceneA);
    REQUIRE(r.isFading() == true);

    for (int i = 0; i < 100; i++) r.updateFade(0.1f);
    REQUIRE(r.isFading() == false);
    REQUIRE(game.getCurrentPlayer() == b);
    REQUIRE(game.getScene() == sceneB);
}

TEST_CASE("switching players preserves each player's own verb selection", "[party]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_state_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_state_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);

    a->setVerb(game.walkVerb);
    b->setVerb(game.pushVerb);

    REQUIRE(game.switchToPlayer(a));
    REQUIRE(game.getCurrentPlayer()->getVerb() == game.walkVerb);

    REQUIRE(game.switchToPlayer(b));
    REQUIRE(game.getCurrentPlayer()->getVerb() == game.pushVerb);

    REQUIRE(game.switchToPlayer(a));
    REQUIRE(game.getCurrentPlayer()->getVerb() == game.walkVerb); // still a's own verb
}

TEST_CASE("switchToNextPlayer/switchToPreviousPlayer move to the immediate neighbor", "[party]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_neighbor_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_neighbor_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);

    game.setCurrentPlayer(a);
    REQUIRE(game.switchToNextPlayer());
    REQUIRE(game.getCurrentPlayer() == b);

    REQUIRE(game.switchToPreviousPlayer());
    REQUIRE(game.getCurrentPlayer() == a);
}

TEST_CASE("switchToNextPlayer cycling through the whole party returns to the start", "[party]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_cycle_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_cycle_b", Vector2<>(0, 0), Vector2<>(10, 10));
    auto c = std::make_shared<Player>("party_cycle_c", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);
    game.addPlayer(c);

    game.setCurrentPlayer(a);
    size_t partySize = game.getPlayers().size(); // includes players from other tests too

    for (size_t i = 0; i < partySize; i++) {
        REQUIRE(game.switchToNextPlayer());
    }
    REQUIRE(game.getCurrentPlayer() == a); // a full cycle wraps back to the start
}

TEST_CASE("switchToNextPlayer fails if the current player isn't part of the party", "[party]") {
    Game& game = Game::getInstance();
    auto stranger = std::make_shared<Player>("party_orphan", Vector2<>(0, 0), Vector2<>(10, 10));
    game.setCurrentPlayer(stranger); // never added via addPlayer
    REQUIRE_FALSE(game.switchToNextPlayer());
    REQUIRE_FALSE(game.switchToPreviousPlayer());
}

TEST_CASE("resolveUIAction: switchPlayer switches to the named party member", "[party][uiaction]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_action_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_action_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);
    game.setCurrentPlayer(a);

    Event switchEvent = resolveUIAction(nlohmann::json{ {"action", "switchPlayer"}, {"name", "party_action_b"} });
    switchEvent.execute();
    REQUIRE(game.getCurrentPlayer() == b);
}

TEST_CASE("resolveUIAction: switchToNextPlayer/switchToPreviousPlayer cycle the party", "[party][uiaction]") {
    Game& game = Game::getInstance();
    auto a = std::make_shared<Player>("party_action_next_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<Player>("party_action_next_b", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(a);
    game.addPlayer(b);
    game.setCurrentPlayer(a);

    Event nextEvent = resolveUIAction(nlohmann::json{ {"action", "switchToNextPlayer"} });
    nextEvent.execute();
    REQUIRE(game.getCurrentPlayer() == b);

    Event prevEvent = resolveUIAction(nlohmann::json{ {"action", "switchToPreviousPlayer"} });
    prevEvent.execute();
    REQUIRE(game.getCurrentPlayer() == a);
}
