#include "catch.hpp"
#include "DialogueOption.hpp"
#include "DialogueNode.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Event.hpp"

// ---------------------------------------------------------------------------
// Helper: drain leftover pending events from previous sections
// ---------------------------------------------------------------------------
static void drainPendingEvents() {
    for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

TEST_CASE("DialogueOption condition default visible", "[dialogue][condition]") {
    drainPendingEvents();
    auto opt = std::make_shared<DialogueOption>("Hello");
    REQUIRE(opt->isVisible() == true);
}

TEST_CASE("DialogueOption isVisible returns true when condition is true", "[dialogue][condition]") {
    drainPendingEvents();
    auto opt = std::make_shared<DialogueOption>("Hello");
    opt->setCondition([]() { return true; });
    REQUIRE(opt->isVisible() == true);
}

TEST_CASE("DialogueOption isVisible returns false when condition is false", "[dialogue][condition]") {
    drainPendingEvents();
    auto opt = std::make_shared<DialogueOption>("Hello");
    opt->setCondition([]() { return false; });
    REQUIRE(opt->isVisible() == false);
}

TEST_CASE("DialogueOption condition based on game state flag", "[dialogue][condition]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto opt = std::make_shared<DialogueOption>("Open door");
    opt->setCondition([&game]() { return game.getGameState().getFlag("has_key"); });
    REQUIRE(opt->isVisible() == false);
    game.getGameState().setFlag("has_key");
    REQUIRE(opt->isVisible() == true);
}

TEST_CASE("DialogueOption condition based on game state counter", "[dialogue][condition]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto opt = std::make_shared<DialogueOption>("Bribe");
    opt->setCondition([&game]() { return game.getGameState().getCounter("gold") > 10; });
    REQUIRE(opt->isVisible() == false);
    game.getGameState().setCounter("gold", 5);
    REQUIRE(opt->isVisible() == false);
    game.getGameState().setCounter("gold", 15);
    REQUIRE(opt->isVisible() == true);
}

TEST_CASE("DialogueNode showOptions skips invisible options in UI", "[dialogue][condition]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    game.getUI().createLayout("dialogue");
    game.getUI().setLayout("dialogue");
    game.getUI().createLayout("afterDialogue");
    game.getUI().setDialogueLayout(&game.getUI().getLayout("dialogue"));
    game.getUI().setAfterDialogueLayout(&game.getUI().getLayout("afterDialogue"));

    auto visibleOpt = std::make_shared<DialogueOption>("Visible");
    visibleOpt->setCondition([]() { return true; });
    auto invisibleOpt = std::make_shared<DialogueOption>("Hidden");
    invisibleOpt->setCondition([]() { return false; });

    auto node = std::make_shared<DialogueNode>(Event([] {}));
    node->setOptions({ visibleOpt, invisibleOpt });

    REQUIRE_NOTHROW(node->showOptions());
}

TEST_CASE("DialogueNode showOptions with all invisible falls through to afterDialogue", "[dialogue][condition]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    game.getUI().createLayout("dialogue");
    game.getUI().setLayout("dialogue");
    game.getUI().createLayout("afterDialogue");
    game.getUI().setDialogueLayout(&game.getUI().getLayout("dialogue"));
    game.getUI().setAfterDialogueLayout(&game.getUI().getLayout("afterDialogue"));

    auto invisibleOpt = std::make_shared<DialogueOption>("Hidden");
    invisibleOpt->setCondition([]() { return false; });

    auto node = std::make_shared<DialogueNode>(Event([] {}));
    node->setOptions({ invisibleOpt });

    player->disableMovement();
    REQUIRE(player->canMove() == false);
    node->showOptions();
    REQUIRE(player->canMove() == true);
}
