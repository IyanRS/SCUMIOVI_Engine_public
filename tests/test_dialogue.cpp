#include "catch.hpp"
#include "Text.hpp"
#include "DialogueNode.hpp"
#include "DialogueOption.hpp"
#include "Event.hpp"
#include <memory>

// ===========================================================================
// Text tests
// ===========================================================================

TEST_CASE("Text default construction", "[text]") {
    Text t;
    REQUIRE(t.toString() == "");
    REQUIRE(t.getSize() == 0);
    REQUIRE(t.hasOutline() == false);
    REQUIRE(t.hasShadow() == false);
    REQUIRE(t.getTextOverMode() == TEXT_ON_TOP);
}

TEST_CASE("Text construction with parameters", "[text]") {
    int color[4] = { 125, 44, 125, 0xff };
    Text t("Hello", 11, color, true, TEXT_CENTERED, true);
    REQUIRE(t.toString() == "Hello");
    REQUIRE(t.getSize() == 11);
    REQUIRE(t.hasOutline() == true);
    REQUIRE(t.hasShadow() == true);
    REQUIRE(t.getTextOverMode() == TEXT_CENTERED);
}

TEST_CASE("Text construction without outline and shadow", "[text]") {
    int color[4] = { 255, 255, 255, 0xff };
    Text t("option text", 10, color);
    REQUIRE(t.toString() == "option text");
    REQUIRE(t.getSize() == 10);
    REQUIRE(t.hasOutline() == false);
    REQUIRE(t.hasShadow() == false);
    REQUIRE(t.getTextOverMode() == TEXT_ON_TOP);
}

TEST_CASE("Text copy construction", "[text]") {
    int color[4] = { 125, 44, 125, 0xff };
    Text original("Hello", 11, color, true, TEXT_TO_LEFT, true);
    Text copy("Goodbye", original);
    REQUIRE(copy.toString() == "Goodbye");
    REQUIRE(copy.getSize() == 11);
    REQUIRE(copy.hasOutline() == true);
    REQUIRE(copy.hasShadow() == true);
    REQUIRE(copy.getTextOverMode() == TEXT_TO_LEFT);
}

TEST_CASE("Text getColor copies to output array", "[text]") {
    int color[4] = { 125, 44, 125, 0xff };
    Text t("Test", 12, color);
    int out[4] = { 0, 0, 0, 0 };
    t.getColor(out);
    REQUIRE(out[0] == 125);
    REQUIRE(out[1] == 44);
    REQUIRE(out[2] == 125);
    REQUIRE(out[3] == 0xff);
}

TEST_CASE("Text toString", "[text]") {
    int color[4] = { 255, 0, 0, 0xff };
    Text t("Hello, world!", 14, color);
    REQUIRE(t.toString() == "Hello, world!");
}

TEST_CASE("Text getTextOverMode returns correct mode", "[text]") {
    int color[4] = { 0, 0, 0, 0 };
    Text t("left", 10, color, false, TEXT_TO_LEFT);
    REQUIRE(t.getTextOverMode() == TEXT_TO_LEFT);
}

TEST_CASE("Text hasOutline and hasShadow flags", "[text]") {
    int color[4] = { 0, 0, 0, 0 };
    Text t("test", 10, color, true, TEXT_CENTERED, true);
    REQUIRE(t.hasOutline() == true);
    REQUIRE(t.hasShadow() == true);
}

// ===========================================================================
// DialogueOption tests
// ===========================================================================

TEST_CASE("DialogueOption construction with text only", "[dialogue]") {
    DialogueOption opt("Hello");
    REQUIRE(opt.getText() == "Hello");
}

TEST_CASE("DialogueOption construction with text and next node", "[dialogue]") {
    auto next = std::make_shared<DialogueNode>(Event([] {}));
    DialogueOption opt("Go to node", next);
    REQUIRE(opt.getText() == "Go to node");
}

TEST_CASE("DialogueOption setNextNode changes the target", "[dialogue]") {
    auto first = std::make_shared<DialogueNode>(Event([] {}));
    auto second = std::make_shared<DialogueNode>(Event([] {}));
    DialogueOption opt("Option", first);
    opt.setNextNode(second);
    REQUIRE(opt.getText() == "Option");
}

TEST_CASE("DialogueOption default next node is nullptr", "[dialogue]") {
    DialogueOption opt("End");
    REQUIRE(opt.getText() == "End");
}

// ===========================================================================
// DialogueNode tests
// ===========================================================================

TEST_CASE("DialogueNode construction", "[dialogue]") {
    DialogueNode node(Event([] {}));
    REQUIRE(node.getOptions().empty());
}

TEST_CASE("DialogueNode setOptions and getOptions", "[dialogue]") {
    auto node = std::make_shared<DialogueNode>(Event([] {}));
    auto opt1 = std::make_shared<DialogueOption>("Opt 1");
    auto opt2 = std::make_shared<DialogueOption>("Opt 2");
    std::vector<std::shared_ptr<DialogueOption>> options = { opt1, opt2 };
    node->setOptions(options);
    REQUIRE(node->getOptions().size() == 2);
    REQUIRE(node->getOptions()[0]->getText() == "Opt 1");
    REQUIRE(node->getOptions()[1]->getText() == "Opt 2");
}

TEST_CASE("DialogueNode options are empty by default", "[dialogue]") {
    DialogueNode node(Event([] {}));
    REQUIRE(node.getOptions().size() == 0);
}
