#include "catch.hpp"
#include "UILoader.hpp"
#include "Game.hpp"
#include "UILayout.hpp"
#include "UIElement.hpp"
#include "Player.hpp"
#include "Localization.hpp"
#include <fstream>
#include <filesystem>
#include <cstdio>

namespace {
	void writeFile(const std::string& path, const std::string& contents) {
		std::ofstream f(path);
		f << contents;
	}
}

TEST_CASE("loadLayout builds elements with position, size and text from JSON", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_basic.json";
    writeFile(path, R"({
        "name": "loader_basic",
        "elements": [
            { "type": "label", "name": "greeting", "position": [10, 20], "text": "Hello" }
        ]
    })");

    REQUIRE(UILoader::loadLayout(game, path));
    UILayout& layout = game.getUI().getLayout("loader_basic");
    auto element = layout.getUIElement("greeting");
    REQUIRE(element != nullptr);
    REQUIRE(element->getPosition().x == 10);
    REQUIRE(element->getPosition().y == 20);
    REQUIRE(element->getTextOver().toString() == "Hello");

    std::remove(path);
}

TEST_CASE("loadLayout returns false for a missing file", "[uiloader]") {
    Game& game = Game::getInstance();
    REQUIRE_FALSE(UILoader::loadLayout(game, "definitely_missing_layout.json"));
}

TEST_CASE("loadLayout returns false for malformed JSON", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_malformed.json";
    writeFile(path, "{ not valid json ");
    REQUIRE_FALSE(UILoader::loadLayout(game, path));
    std::remove(path);
}

TEST_CASE("loadLayout returns false when \"name\" is missing", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_noname.json";
    writeFile(path, R"({ "elements": [] })");
    REQUIRE_FALSE(UILoader::loadLayout(game, path));
    std::remove(path);
}

TEST_CASE("a button's onClick object action runs the named script", "[uiloader]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("loader_btn_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int ran = 0;
    game.registerScript("loader_test_script", [&ran]() { ran++; });

    const char* path = "test_layout_button.json";
    writeFile(path, R"({
        "name": "loader_button_test",
        "elements": [
            { "type": "button", "name": "playBtn", "position": [0,0], "size": [40,20],
              "text": "Play", "onClick": { "action": "script", "name": "loader_test_script" } }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));

    auto button = game.getUI().getLayout("loader_button_test").getUIElement("playBtn");
    REQUIRE(button != nullptr);
    button->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(ran == 1);

    std::remove(path);
}

TEST_CASE("a button's onClick string shorthand runs the named script", "[uiloader]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("loader_btn_player2", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    int ran = 0;
    game.registerScript("loader_shorthand_script", [&ran]() { ran++; });

    const char* path = "test_layout_button_shorthand.json";
    writeFile(path, R"({
        "name": "loader_button_shorthand_test",
        "elements": [
            { "type": "button", "name": "btn2", "size": [40,20], "text": "Go",
              "onClick": "loader_shorthand_script" }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));

    auto button = game.getUI().getLayout("loader_button_shorthand_test").getUIElement("btn2");
    REQUIRE(button != nullptr);
    button->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(ran == 1);

    std::remove(path);
}

TEST_CASE("built-in actions: openLayout switches the base layout", "[uiloader]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("loader_action_target");

    const char* path = "test_layout_openlayout.json";
    writeFile(path, R"({
        "name": "loader_action_source",
        "elements": [
            { "type": "button", "size": [10,10], "text": "Go",
              "onClick": { "action": "openLayout", "name": "loader_action_target" } }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    ui.setLayout("loader_action_source");

    auto button = ui.getLayout()->getUIElements()[0];
    button->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(ui.getLayout()->getName() == "loader_action_target");

    std::remove(path);
}

TEST_CASE("built-in actions: openOverlay and closeOverlay control the overlay", "[uiloader]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("loader_overlay_base");
    ui.createLayout("loader_overlay_menu");
    ui.setLayout("loader_overlay_base");

    const char* path = "test_layout_overlay_actions.json";
    writeFile(path, R"({
        "name": "loader_overlay_menu_content",
        "elements": [
            { "type": "button", "name": "openBtn", "size": [10,10], "text": "Open",
              "onClick": { "action": "openOverlay", "name": "loader_overlay_menu" } },
            { "type": "button", "name": "closeBtn", "size": [10,10], "text": "Close",
              "onClick": { "action": "closeOverlay" } }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    ui.setLayout("loader_overlay_menu_content");

    auto openBtn = ui.getLayout()->getUIElement("openBtn");
    auto closeBtn = ui.getLayout()->getUIElement("closeBtn");
    REQUIRE(openBtn != nullptr);
    REQUIRE(closeBtn != nullptr);

    openBtn->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(ui.getOverlay() != nullptr);
    REQUIRE(ui.isGamePaused());

    closeBtn->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(ui.getOverlay() == nullptr);

    std::remove(path);
}

TEST_CASE("built-in action: setLocale switches the active locale", "[uiloader]") {
    Game& game = Game::getInstance();
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.addTranslation("es", "Hola", "Hola-es-marker");
    loc.setLocale("en");

    const char* path = "test_layout_setlocale.json";
    writeFile(path, R"({
        "name": "loader_setlocale_test",
        "elements": [
            { "type": "button", "size": [10,10], "text": "ES",
              "onClick": { "action": "setLocale", "locale": "es" } }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    UILayout& layout = game.getUI().getLayout("loader_setlocale_test");
    auto button = layout.getUIElements()[0];

    button->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(game.getLocale() == "es");

    game.setLocale("en");
    std::remove(path);
}

TEST_CASE("a JSON label re-resolves its text when the locale changes", "[uiloader]") {
    Game& game = Game::getInstance();
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.addTranslation("es", "Play", "Jugar");
    loc.setLocale("en");

    const char* path = "test_layout_localized_label.json";
    writeFile(path, R"({
        "name": "loader_localized_label_test",
        "elements": [
            { "type": "label", "name": "playLabel", "text": "Play" }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    auto label = game.getUI().getLayout("loader_localized_label_test").getUIElement("playLabel");
    REQUIRE(label != nullptr);
    REQUIRE(label->getTextOver().toString() == "Play");

    loc.setLocale("es");
    std::dynamic_pointer_cast<UIElement>(label)->refresh();
    REQUIRE(label->getTextOver().toString() == "Jugar");

    loc.setLocale("en");
    std::remove(path);
}

TEST_CASE("nested panels build a real child tree reachable by name", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_nested.json";
    writeFile(path, R"({
        "name": "loader_nested_test",
        "elements": [
            { "type": "panel", "name": "outerPanel", "position": [10,10], "size": [100,50],
              "children": [
                { "type": "label", "name": "innerLabel", "position": [5,5], "text": "Inside" }
              ]
            }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    UILayout& layout = game.getUI().getLayout("loader_nested_test");
    auto inner = layout.getUIElement("innerLabel");
    REQUIRE(inner != nullptr);

    layout.getRenderElements();
    REQUIRE(inner->getPosition().x == 15); // panel(10) + child offset(5)
    REQUIRE(inner->getPosition().y == 15);

    std::remove(path);
}

TEST_CASE("verbGrid and inventoryGrid elements are built via the layout's grid helpers", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_grids.json";
    writeFile(path, R"({
        "name": "loader_grids_test",
        "elements": [
            { "type": "verbGrid", "position": [0,0], "verbs": [["walk","talk"]] },
            { "type": "inventoryGrid", "position": [100,0], "columns": 2, "rows": 1 }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    UILayout& layout = game.getUI().getLayout("loader_grids_test");
    REQUIRE(layout.getUIElements().size() == 2); // one verb grid + one inventory grid, top-level
    std::remove(path);
}

TEST_CASE("a color array texture builds a solid-color Texture", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_colortex.json";
    writeFile(path, R"({
        "name": "loader_colortex_test",
        "elements": [
            { "type": "image", "name": "colorBox", "position": [0,0], "size": [10,10], "texture": [10, 20, 30, 255] }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    auto element = game.getUI().getLayout("loader_colortex_test").getUIElement("colorBox");
    REQUIRE(element != nullptr);
    auto texture = element->getTexture();
    REQUIRE(texture != nullptr);
    int color[4];
    texture->getColor(color);
    REQUIRE(color[0] == 10);
    REQUIRE(color[1] == 20);
    REQUIRE(color[2] == 30);

    std::remove(path);
}

TEST_CASE("an anchor field positions an element the same way the C++ API does", "[uiloader]") {
    Game& game = Game::getInstance();
    const char* path = "test_layout_anchor.json";
    writeFile(path, R"({
        "name": "loader_anchor_test",
        "elements": [
            { "type": "image", "name": "centered", "size": [20,10], "anchor": "center" }
        ]
    })");
    REQUIRE(UILoader::loadLayout(game, path));
    UILayout& layout = game.getUI().getLayout("loader_anchor_test");
    layout.getRenderElements();
    auto element = layout.getUIElement("centered");
    REQUIRE(element->getPosition().x == 150); // (320/2) - (20/2)
    REQUIRE(element->getPosition().y == 95);  // (200/2) - (10/2)

    std::remove(path);
}

TEST_CASE("loadLayoutsFromDirectory loads every json file in a directory", "[uiloader]") {
    Game& game = Game::getInstance();
    std::filesystem::path dir = "test_ui_dir";
    std::filesystem::create_directories(dir);
    writeFile((dir / "a.json").string(), R"({"name": "loader_dir_a", "elements": []})");
    writeFile((dir / "b.json").string(), R"({"name": "loader_dir_b", "elements": []})");
    writeFile((dir / "notjson.txt").string(), "ignored");

    REQUIRE(UILoader::loadLayoutsFromDirectory(game, dir.string()));
    REQUIRE_NOTHROW(game.getUI().getLayout("loader_dir_a"));
    REQUIRE_NOTHROW(game.getUI().getLayout("loader_dir_b"));

    std::filesystem::remove_all(dir);
}

TEST_CASE("loadLayoutsFromDirectory returns false for a missing directory", "[uiloader]") {
    Game& game = Game::getInstance();
    REQUIRE_FALSE(UILoader::loadLayoutsFromDirectory(game, "definitely_missing_directory"));
}
