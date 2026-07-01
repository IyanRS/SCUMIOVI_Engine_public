#include "catch.hpp"
#include "UI.hpp"
#include "UILayout.hpp"
#include "Event.hpp"
#include "Game.hpp"
#include "Player.hpp"

static void drainPendingEvents() {
    for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

TEST_CASE("UI layout lifecycle with unique_ptr", "[ui]") {
    drainPendingEvents();
    UI ui;

    SECTION("create and retrieve layout by name") {
        ui.createLayout("test_layout");
        REQUIRE(&ui.getLayout("test_layout") != nullptr);
    }

    SECTION("multiple layouts are independent") {
        ui.createLayout("layout_a");
        ui.createLayout("layout_b");
        REQUIRE(ui.getLayout("layout_a").getName() == "layout_a");
        REQUIRE(ui.getLayout("layout_b").getName() == "layout_b");
    }

    SECTION("setLayout switches current layout") {
        ui.createLayout("first");
        ui.createLayout("second");
        ui.setLayout("first");
        REQUIRE(ui.getLayout() != nullptr);
        REQUIRE(ui.getLayout()->getName() == "first");
        ui.setLayout("second");
        REQUIRE(ui.getLayout()->getName() == "second");
    }

    SECTION("getLayout returns nullptr when no layout set") {
        REQUIRE(ui.getLayout() == nullptr);
    }

    SECTION("updateUI with no layout does not crash") {
        REQUIRE_NOTHROW(ui.updateUI());
    }

    SECTION("createLayout does not set layout as current") {
        ui.createLayout("main");
        REQUIRE(ui.getLayout() == nullptr);
    }
}

TEST_CASE("UILayout inventory sliding window", "[ui][inventory]") {
    drainPendingEvents();
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("ui_test_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    UI& ui = game.getUI();
    ui.createLayout("inv_layout");
    ui.setLayout("inv_layout");
    UILayout& layout = *ui.getLayout();

    Event onUIUpdate([]{});

    SECTION("inventory index starts at 0") {
        REQUIRE(layout.getInventoryIndex() == 0);
    }

    SECTION("incrementInventoryIndex advances the index") {
        auto& inv = player->getInventory();
        for (int i = 0; i < 100; i++) {
            inv.addItem(std::make_shared<GameObject>("item" + std::to_string(i), Vector2<>(0, 0)));
        }
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);

        layout.incrementInventoryIndex(4);
        REQUIRE(layout.getInventoryIndex() == 4);
        layout.incrementInventoryIndex(2);
        REQUIRE(layout.getInventoryIndex() == 6);
    }

    SECTION("incrementInventoryIndex accepts negative values") {
        auto& inv = player->getInventory();
        for (int i = 0; i < 100; i++) {
            inv.addItem(std::make_shared<GameObject>("item" + std::to_string(i), Vector2<>(0, 0)));
        }
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);

        layout.incrementInventoryIndex(4);
        layout.incrementInventoryIndex(-2);
        REQUIRE(layout.getInventoryIndex() == 2);
    }

    SECTION("createInventoryMatrix does not crash") {
        REQUIRE_NOTHROW(layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate));
    }

    SECTION("updateUI with empty inventory does not crash") {
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);
        REQUIRE_NOTHROW(layout.updateUI());
    }

    SECTION("updateUI after advancing index past empty inventory does not crash") {
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);
        layout.incrementInventoryIndex(100);
        REQUIRE_NOTHROW(layout.updateUI());
    }

    SECTION("items are placed into slots via sliding window") {
        auto& inv = player->getInventory();
        for (int i = 0; i < 10; i++) {
            auto item = std::make_shared<GameObject>("invitem" + std::to_string(i), Vector2<>(0, 0));
            item->addTag("item");
            inv.addItem(item);
        }
        REQUIRE(inv.getItems().size() == 10);

        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);

        // Index 0: items 0-7 should be placed (8 slots)
        REQUIRE_NOTHROW(layout.updateUI());
        REQUIRE(layout.getInventoryIndex() == 0);

        // Advance by 1: items 1-8
        layout.incrementInventoryIndex(1);
        REQUIRE(layout.getInventoryIndex() == 1);
        REQUIRE_NOTHROW(layout.updateUI());

        // Advance by 1: items 2-9
        layout.incrementInventoryIndex(1);
        REQUIRE(layout.getInventoryIndex() == 2);
        REQUIRE_NOTHROW(layout.updateUI());
    }

    SECTION("clamping prevents scrolling past last item") {
        auto& inv = player->getInventory();
        for (int i = 0; i < 10; i++) {
            auto item = std::make_shared<GameObject>("invitem" + std::to_string(i), Vector2<>(0, 0));
            inv.addItem(item);
        }
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);

        // With 10 items and 8 slots, max index = 10 - 8 = 2
        layout.incrementInventoryIndex(100);
        REQUIRE(layout.getInventoryIndex() == 2);
    }

    SECTION("clamping prevents negative index") {
        auto& inv = player->getInventory();
        for (int i = 0; i < 10; i++) {
            auto item = std::make_shared<GameObject>("invitem" + std::to_string(i), Vector2<>(0, 0));
            inv.addItem(item);
        }
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);

        layout.incrementInventoryIndex(1);
        REQUIRE(layout.getInventoryIndex() == 1);
        layout.incrementInventoryIndex(-100);
        REQUIRE(layout.getInventoryIndex() == 0);
    }

    SECTION("clamping with empty inventory stays at 0") {
        layout.createInventoryMatrix(Vector2<>(0, 0), 4, 2, nullptr, onUIUpdate);

        layout.incrementInventoryIndex(100);
        REQUIRE(layout.getInventoryIndex() == 0);
        layout.incrementInventoryIndex(-100);
        REQUIRE(layout.getInventoryIndex() == 0);
    }
}
