#include "catch.hpp"
#include "UIElement.hpp"
#include "UILayout.hpp"
#include "UIInfoLabel.hpp"
#include "UIVerbGrid.hpp"
#include "UIInventoryGrid.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Text.hpp"

TEST_CASE("UIPanel stores children", "[ui]") {
    auto panel = std::make_shared<UIPanel>("panel", Vector2<>(0, 0), Vector2<>(100, 50));
    auto img = std::make_shared<UIImage>("img", Vector2<>(5, 5), Vector2<>(10, 10), nullptr);
    panel->addChild(img);
    REQUIRE(panel->getChildren().size() == 1);
    REQUIRE(panel->getChildren()[0] == img);
}

TEST_CASE("getRenderElements flattens the tree and resolves absolute positions", "[ui]") {
    UILayout layout("flatten_test");
    auto panel = std::make_shared<UIPanel>("panel", Vector2<>(10, 20), Vector2<>(100, 50));
    auto child = std::make_shared<UIImage>("childImg", Vector2<>(5, 5), Vector2<>(10, 10), nullptr);
    panel->addChild(child);
    layout.addUIElement(panel);

    auto flat = layout.getRenderElements();
    REQUIRE(flat.size() == 2);              // panel + its child
    REQUIRE(panel->getPosition().x == 10);  // top-level: absolute == offset
    REQUIRE(panel->getPosition().y == 20);
    REQUIRE(child->getPosition().x == 15);  // child: parent + offset
    REQUIRE(child->getPosition().y == 25);
}

TEST_CASE("plain GameObject elements pass through the flattener unchanged", "[ui]") {
    UILayout layout("mixed_test");
    auto plain = std::make_shared<GameObject>("plain", Vector2<>(7, 8), Vector2<>(1, 1));
    layout.addUIElement(plain);
    auto flat = layout.getRenderElements();
    REQUIRE(flat.size() == 1);
    REQUIRE(flat[0] == plain);
    REQUIRE(plain->getPosition().x == 7);
}

TEST_CASE("getUIElement finds nested elements by name", "[ui]") {
    UILayout layout("find_test");
    auto panel = std::make_shared<UIPanel>("panel2", Vector2<>(0, 0), Vector2<>(10, 10));
    auto child = std::make_shared<UILabel>("nestedLabel", Vector2<>(1, 1), Text());
    panel->addChild(child);
    layout.addUIElement(panel);
    REQUIRE(layout.getUIElement("nestedLabel") == child);
    REQUIRE(layout.getUIElement("missing") == nullptr);
}

TEST_CASE("UIButton runs its onClick on any click", "[ui]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("ui_btn_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    bool clicked = false;
    int color[4] = { 255, 255, 255, 255 };
    Text label = Text("Play", 11, color);
    auto btn = std::make_shared<UIButton>("btn", Vector2<>(0, 0), Vector2<>(40, 20), nullptr, label,
        Event([&clicked]() { clicked = true; }));

    btn->onClick(Vector2<>(1, 1), game.anyVerb);
    REQUIRE(clicked);
}

TEST_CASE("refresh recurses into children", "[ui]") {
    struct Counter : public UIElement {
        int* counter;
        Counter(int* c) : UIElement("counter"), counter(c) {}
        void refresh() override { (*counter)++; UIElement::refresh(); }
    };

    int count = 0;
    auto panel = std::make_shared<UIPanel>("p", Vector2<>(0, 0), Vector2<>(10, 10));
    panel->addChild(std::make_shared<Counter>(&count));
    panel->refresh();
    REQUIRE(count == 1);
}

TEST_CASE("UIInfoLabel refreshes its text from the current player's verb info", "[ui]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("info_label_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);
    player->setVerb(game.walkVerb);

    int color[4] = { 125, 44, 125, 255 };
    Text format = Text("", 11, color, false, TEXT_CENTERED);
    UIInfoLabel label("infoLabel", Vector2<>(0, 0), format);

    label.refresh();
    REQUIRE(label.getTextOver().toString() == game.walkVerb->getInfo(nullptr, nullptr));
}

TEST_CASE("UIVerbGrid highlights the button matching the layout's highlighted verb", "[ui]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("verbgrid_test");
    ui.setLayout("verbgrid_test");
    UILayout& layout = *ui.getLayout();

    auto grid = std::make_shared<UIVerbGrid>("grid", Vector2<>(0, 0));
    auto walkButton = std::make_shared<UIVerbButton>(game.walkVerb, Vector2<>(0, 0), Vector2<>(20, 10), nullptr);
    auto talkButton = std::make_shared<UIVerbButton>(game.talkVerb, Vector2<>(0, 10), Vector2<>(20, 10), nullptr);
    grid->addButton(walkButton);
    grid->addButton(talkButton);
    layout.addUIElement(grid);

    layout.setHighlightedVerb(game.walkVerb);
    grid->refresh();

    int walkColor[4], talkColor[4];
    walkButton->getTextOver().getColor(walkColor);
    talkButton->getTextOver().getColor(talkColor);
    REQUIRE(walkColor[0] == 255); // highlighted -> white
    REQUIRE(talkColor[0] != 255); // not highlighted -> back to its format color

    layout.setHighlightedVerb(nullptr);
    grid->refresh();
    walkButton->getTextOver().getColor(walkColor);
    REQUIRE(walkColor[0] != 255);
}

TEST_CASE("Hovering a UIVerbButton highlights it and leaving restores it", "[ui]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("verb_hover_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    UI& ui = game.getUI();
    ui.createLayout("verbhover_test");
    ui.setLayout("verbhover_test");
    UILayout& layout = *ui.getLayout();

    auto grid = std::make_shared<UIVerbGrid>("grid", Vector2<>(0, 0));
    auto pushButton = std::make_shared<UIVerbButton>(game.pushVerb, Vector2<>(0, 0), Vector2<>(20, 10), nullptr);
    grid->addButton(pushButton);
    layout.addUIElement(grid);

    // The full hover path (onMouseOver, not highlight() directly): the
    // button's own highlight must survive whatever else the composed hover
    // event does — a layout repaint from here once wiped it immediately.
    pushButton->onMouseOver(Vector2<>(5, 5));
    int color[4];
    pushButton->getTextOver().getColor(color);
    REQUIRE(color[0] == 255); // highlighted -> white

    pushButton->onMouseNotOver(Vector2<>(50, 50));
    pushButton->getTextOver().getColor(color);
    REQUIRE(color[0] != 255); // back to its format color

    // Hovering the button never makes it the sentence target either.
    pushButton->onMouseOver(Vector2<>(5, 5));
    REQUIRE(player->getTargetObject() == nullptr);
}

TEST_CASE("UIInventoryGrid places items into slots using its own configured geometry", "[ui]") {
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("inv_grid_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    UI& ui = game.getUI();
    ui.createLayout("invgrid_test");
    ui.setLayout("invgrid_test");

    auto& inv = player->getInventory();
    auto item0 = std::make_shared<GameObject>("gridItem0", Vector2<>(0, 0), Vector2<>(8, 8));
    item0->addTag("item");
    inv.addItem(item0);

    auto grid = std::make_shared<UIInventoryGrid>("grid", Vector2<>(100, 200), 4, 2, Vector2<>(40, 22), nullptr);
    REQUIRE(grid->getItemSlots().size() == 8);
    REQUIRE(grid->getChildren().size() == 8); // just the slots so far

    grid->refresh();

    // item0 goes into slot 0, whose offset is (0,0) relative to the grid.
    REQUIRE(grid->getChildren().size() == 9); // 8 slots + 1 item
    Vector2<> expectedCenter = grid->getItemSlots()[0]->getSize() / 2 - item0->getSize() / 2;
    Vector2<> expected = grid->getOffset() + grid->getItemSlots()[0]->getOffset() + expectedCenter;
    REQUIRE(item0->getPosition().x == expected.x);
    REQUIRE(item0->getPosition().y == expected.y);

    // A second refresh should not duplicate the item (stale "item" children are dropped first).
    grid->refresh();
    REQUIRE(grid->getChildren().size() == 9);
}

TEST_CASE("TopLeft anchor (default) matches plain offset positioning", "[ui][anchor]") {
    UILayout layout("anchor_topleft_test");
    auto el = std::make_shared<UIImage>("el", Vector2<>(5, 7), Vector2<>(20, 10), nullptr);
    layout.addUIElement(el);
    layout.getRenderElements();
    REQUIRE(el->getPosition().x == 5);
    REQUIRE(el->getPosition().y == 7);
}

TEST_CASE("Center-anchored elements resolve relative to the screen", "[ui][anchor]") {
    UILayout layout("anchor_center_test");
    auto centered = std::make_shared<UIImage>("centered", Vector2<>(0, 0), Vector2<>(20, 10), nullptr);
    centered->setAnchor(UIAnchor::Center);
    layout.addUIElement(centered);

    layout.getRenderElements();
    // Screen is 320x200 (RENDER_WIDTH/HEIGHT): anchor point (160,100) minus
    // half the element's own size (10,5) = (150, 95).
    REQUIRE(centered->getPosition().x == 150);
    REQUIRE(centered->getPosition().y == 95);
}

TEST_CASE("BottomRight-anchored elements sit flush with the screen edge", "[ui][anchor]") {
    UILayout layout("anchor_bottomright_test");
    auto br = std::make_shared<UIImage>("br", Vector2<>(0, 0), Vector2<>(20, 10), nullptr);
    br->setAnchor(UIAnchor::BottomRight);
    layout.addUIElement(br);

    layout.getRenderElements();
    // anchor point (320,200) minus the element's own size (20,10) = (300, 190).
    REQUIRE(br->getPosition().x == 300);
    REQUIRE(br->getPosition().y == 190);
}

TEST_CASE("A non-default offset nudges an anchored element from its anchor point", "[ui][anchor]") {
    UILayout layout("anchor_offset_test");
    auto el = std::make_shared<UIImage>("el", Vector2<>(3, -4), Vector2<>(20, 10), nullptr);
    el->setAnchor(UIAnchor::TopRight);
    layout.addUIElement(el);

    layout.getRenderElements();
    // anchor point (320,0) minus own size.x (20) + offset(3,-4) = (303, -4).
    REQUIRE(el->getPosition().x == 303);
    REQUIRE(el->getPosition().y == -4);
}

TEST_CASE("Anchored nested children resolve relative to their parent's size", "[ui][anchor]") {
    UILayout layout("anchor_nested_test");
    auto panel = std::make_shared<UIPanel>("panel", Vector2<>(50, 50), Vector2<>(100, 40));
    auto child = std::make_shared<UIImage>("child", Vector2<>(0, 0), Vector2<>(10, 10), nullptr);
    child->setAnchor(UIAnchor::BottomRight);
    panel->addChild(child);
    layout.addUIElement(panel);

    layout.getRenderElements();
    // panel absolute = (50,50); child anchor point relative to the panel's own
    // size (100,40) minus the child's own size (10,10) = (90,30); + panel = (140,80).
    REQUIRE(child->getPosition().x == 140);
    REQUIRE(child->getPosition().y == 80);
}

TEST_CASE("UI overlay renders on top of the base layout and can pause the game", "[ui][overlay]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("overlay_base");
    ui.createLayout("overlay_pause_menu");
    ui.setLayout("overlay_base");

    auto baseEl = std::make_shared<GameObject>("baseEl", Vector2<>(0, 0), Vector2<>(1, 1));
    ui.getLayout("overlay_base").addUIElement(baseEl);
    auto overlayEl = std::make_shared<GameObject>("overlayEl", Vector2<>(0, 0), Vector2<>(1, 1));
    ui.getLayout("overlay_pause_menu").addUIElement(overlayEl);

    REQUIRE(ui.getOverlay() == nullptr);
    REQUIRE_FALSE(ui.isGamePaused());
    REQUIRE(ui.getRenderElements().size() == 1); // just the base layout

    ui.openOverlay("overlay_pause_menu");
    REQUIRE(ui.getOverlay() != nullptr);
    REQUIRE(ui.isGamePaused());

    auto elements = ui.getRenderElements();
    REQUIRE(elements.size() == 2);
    REQUIRE(elements[0] == baseEl);     // base drawn first
    REQUIRE(elements[1] == overlayEl);  // overlay drawn on top / checked first for clicks

    ui.closeOverlay();
    REQUIRE(ui.getOverlay() == nullptr);
    REQUIRE_FALSE(ui.isGamePaused());
    REQUIRE(ui.getRenderElements().size() == 1);
}

TEST_CASE("openOverlay with pauseGame=false shows the overlay without pausing", "[ui][overlay]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    ui.createLayout("overlay_nonpause_base");
    ui.createLayout("overlay_nonpause_menu");
    ui.setLayout("overlay_nonpause_base");

    ui.openOverlay("overlay_nonpause_menu", false);
    REQUIRE(ui.getOverlay() != nullptr);
    REQUIRE_FALSE(ui.isGamePaused());
    ui.closeOverlay();
}

TEST_CASE("openOverlay with an unknown layout name is a safe no-op", "[ui][overlay]") {
    Game& game = Game::getInstance();
    UI& ui = game.getUI();
    REQUIRE_NOTHROW(ui.openOverlay("definitely_missing_layout"));
    REQUIRE(ui.getOverlay() == nullptr);
}
