#include "catch.hpp"
#include "KeyBindings.hpp"
#include "UIAction.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "UI.hpp"
#include "UILayout.hpp"
#include "Localization.hpp"
#include <fstream>
#include <cstdio>

namespace {
	void writeFile(const std::string& path, const std::string& contents) {
		std::ofstream f(path);
		f << contents;
	}
}

TEST_CASE("KeyBindings binds a recognized key and runs its action", "[keybindings]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("kb_player_1", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    KeyBindings kb;
    int ran = 0;
    REQUIRE(kb.bind("F5", Event([&ran]() { ran++; })));
    REQUIRE(kb.hasBinding(SDLK_F5));

    REQUIRE(kb.handleKeyDown(SDLK_F5));
    REQUIRE(ran == 1);
}

TEST_CASE("KeyBindings::bind fails for an unrecognized key name", "[keybindings]") {
    KeyBindings kb;
    REQUIRE_FALSE(kb.bind("NotARealKeyName", Event()));
}

TEST_CASE("KeyBindings::handleKeyDown for an unbound key is a safe no-op", "[keybindings]") {
    KeyBindings kb;
    REQUIRE_FALSE(kb.handleKeyDown(SDLK_F9));
}

TEST_CASE("KeyBindings::loadFromFile binds every entry in a JSON file", "[keybindings]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("kb_player_2", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    game.registerScript("kb_test_script", []() {});

    const char* path = "test_keybindings_basic.json";
    writeFile(path, R"({
        "F5": { "action": "save", "slot": 0 },
        "F9": { "action": "load", "slot": 0 },
        "A": "kb_test_script"
    })");

    KeyBindings kb;
    REQUIRE(kb.loadFromFile(path));
    REQUIRE(kb.hasBinding(SDLK_F5));
    REQUIRE(kb.hasBinding(SDLK_F9));
    REQUIRE(kb.hasBinding(SDLK_A));

    std::remove(path);
}

TEST_CASE("KeyBindings::loadFromFile returns false for a missing file", "[keybindings]") {
    KeyBindings kb;
    REQUIRE_FALSE(kb.loadFromFile("definitely_missing_keybindings.json"));
}

TEST_CASE("KeyBindings::loadFromFile returns false for malformed JSON", "[keybindings]") {
    const char* path = "test_keybindings_malformed.json";
    writeFile(path, "{ not valid json");
    KeyBindings kb;
    REQUIRE_FALSE(kb.loadFromFile(path));
    std::remove(path);
}

TEST_CASE("KeyBindings::loadFromFile skips unrecognized key names but keeps valid ones", "[keybindings]") {
    const char* path = "test_keybindings_partial.json";
    writeFile(path, R"({
        "F5": { "action": "quit" },
        "NotARealKeyName": { "action": "quit" }
    })");
    KeyBindings kb;
    REQUIRE(kb.loadFromFile(path));
    REQUIRE(kb.hasBinding(SDLK_F5));
    std::remove(path);
}

TEST_CASE("resolveUIAction: toggleOverlay opens when closed and closes when already showing", "[keybindings][uiaction]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("kb_toggle_base");
    ui.createLayout("kb_toggle_menu");
    ui.setLayout("kb_toggle_base");

    nlohmann::json actionJson = { {"action", "toggleOverlay"}, {"name", "kb_toggle_menu"} };
    Event toggle = resolveUIAction(actionJson);

    REQUIRE(ui.getOverlay() == nullptr);
    toggle.execute();
    REQUIRE(ui.getOverlay() != nullptr);
    REQUIRE(ui.getOverlay()->getName() == "kb_toggle_menu");

    toggle.execute();
    REQUIRE(ui.getOverlay() == nullptr);
}

TEST_CASE("resolveUIAction: toggleOverlay switches to a different overlay instead of closing it", "[keybindings][uiaction]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("kb_toggle_base2");
    ui.createLayout("kb_toggle_menu_a");
    ui.createLayout("kb_toggle_menu_b");
    ui.setLayout("kb_toggle_base2");

    ui.openOverlay("kb_toggle_menu_a");
    Event toggleB = resolveUIAction(nlohmann::json{ {"action", "toggleOverlay"}, {"name", "kb_toggle_menu_b"} });
    toggleB.execute();
    REQUIRE(ui.getOverlay()->getName() == "kb_toggle_menu_b");

    ui.closeOverlay();
}

TEST_CASE("resolveUIAction: quit sets the game to stop running", "[keybindings][uiaction]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("kb_quit_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    Event quitEvent = resolveUIAction(nlohmann::json{ {"action", "quit"} });
    REQUIRE_NOTHROW(quitEvent.execute());
    // quitGame() just flips an internal running flag consumed by the main
    // loop; there's no public getter, so we only assert it doesn't throw.
}

TEST_CASE("resolveUIAction: unknown action string is a safe no-op", "[keybindings][uiaction]") {
    Event unknown = resolveUIAction(nlohmann::json{ {"action", "definitely_not_a_real_action"} });
    REQUIRE_NOTHROW(unknown.execute());
}
