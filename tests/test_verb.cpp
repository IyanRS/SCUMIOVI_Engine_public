#include "catch.hpp"
#include "Verb.hpp"
#include "VerbSyntax.hpp"
#include "GameObject.hpp"
#include "Localization.hpp"

TEST_CASE("Verb default construction", "[verb]") {
    Verb v;
    REQUIRE(v.getName() == "");
    REQUIRE(v.getDisplayName() == "");
    REQUIRE(v.RequiresSelectedObject() == false);
}

TEST_CASE("Verb construction with name", "[verb]") {
    Verb v("walk");
    REQUIRE(v.getName() == "walk");
    REQUIRE(v.getDisplayName() == "walk");
}

TEST_CASE("Verb construction with name and displayName", "[verb]") {
    Verb v("walk", "Walk to");
    REQUIRE(v.getName() == "walk");
    REQUIRE(v.getDisplayName() == "Walk to");
}

TEST_CASE("Verb construction with full params", "[verb]") {
    VerbSyntax syntax({ "Use", "Use $target" });
    Verb v("use", "Use", syntax, true);
    REQUIRE(v.getName() == "use");
    REQUIRE(v.getDisplayName() == "Use");
    REQUIRE(v.RequiresSelectedObject() == true);
}

TEST_CASE("Verb equality by name", "[verb]") {
    Verb a("walk");
    Verb b("walk");
    Verb c("push");
    REQUIRE(a == b);
    REQUIRE(a != c);
}

TEST_CASE("Verb default event", "[verb]") {
    Verb v("test");
    bool fired = false;
    Event e([&fired]() { fired = true; });
    v.setDefaultEvent(e);
    Event retrieved = v.getDefaultEvent();
    REQUIRE(&retrieved != &e);
}

TEST_CASE("Verb before and after actual events", "[verb]") {
    Verb v("test");
    bool beforeFired = false;
    bool afterFired = false;
    v.setBeforeActualEvent(Event([&beforeFired]() { beforeFired = true; }));
    v.setAfterActualEvent(Event([&afterFired]() { afterFired = true; }));
    Event before = v.getBeforeActualEvent();
    Event after = v.getAfterActualEvent();
    REQUIRE(&before != &after);
}

TEST_CASE("Verb static defaultDefaultEvent", "[verb]") {
    bool fired = false;
    Verb::setDefaultDefaultEvent(Event([&fired]() { fired = true; }));
    Verb v;
    Event defaultEvent = v.getDefaultEvent();
    Verb::setDefaultDefaultEvent(Event());
}

TEST_CASE("Verb getInfo with syntax using $object0", "[verb]") {
    VerbSyntax syntax({ "Push", "Push $object0" });
    Verb v("push", "Push", syntax, false);

    auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0));
    std::string info = v.getInfo({ obj });
    REQUIRE(info == "Push box");
}

TEST_CASE("Verb getInfo with selected and target", "[verb]") {
    VerbSyntax syntax({ "Give", "Give $target", "Give $selected to", "Give $selected to $target" });
    Verb v("give", "Give", syntax, true);

    auto selected = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    auto target = std::make_shared<GameObject>("door", Vector2<>(0, 0));
    std::string info = v.getInfo(selected, target);
    REQUIRE(info == "Give key to door");
}

TEST_CASE("Verb getInfo without syntax uses displayName", "[verb]") {
    Verb v("walk", "Walk to");
    auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0));
    std::string info = v.getInfo({ obj });
    REQUIRE(info == "Walk to");
}

TEST_CASE("VerbSyntax default construction", "[verbsyntax]") {
    VerbSyntax vs;
    REQUIRE(vs.getStringsCount() == 0);
}

TEST_CASE("VerbSyntax with strings", "[verbsyntax]") {
    VerbSyntax vs({ "Push", "Push $target" });
    REQUIRE(vs.getStringsCount() == 2);
}

TEST_CASE("VerbSyntax getInfo with $object placeholders", "[verbsyntax]") {
    VerbSyntax vs({ "Push", "Push $object0" });
    auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0));
    std::string info = vs.getInfo({ obj });
    REQUIRE(info == "Push box");
}

TEST_CASE("VerbSyntax getInfo with $selected and $target", "[verbsyntax]") {
    VerbSyntax vs({ "Give", "Give $selected to $target" });
    auto selected = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    auto target = std::make_shared<GameObject>("door", Vector2<>(0, 0));
    std::string info = vs.getInfo(selected, target);
    REQUIRE(info == "Give key to door");
}

TEST_CASE("VerbSyntax getInfo with only selected", "[verbsyntax]") {
    VerbSyntax vs({ "Give", "Give $selected to $target", "Give $selected" });
    auto selected = std::make_shared<GameObject>("apple", Vector2<>(0, 0));
    std::string info = vs.getInfo(selected, nullptr);
    REQUIRE(info == "Give apple");
}

TEST_CASE("VerbSyntax getInfo with no matching syntax", "[verbsyntax]") {
    VerbSyntax vs({ "Push $object0", "Push $object0 $object1" });
    std::string info = vs.getInfo({});
    REQUIRE(info == "Couldn't find syntax");
}

TEST_CASE("VerbSyntax copy construction", "[verbsyntax]") {
    VerbSyntax original({ "Use", "Use $target" });
    VerbSyntax copy(original);
    REQUIRE(copy.getStringsCount() == 2);
}

// ---------------------------------------------------------------------------
// Display-name auto-casing in syntax lines
// ---------------------------------------------------------------------------

TEST_CASE("VerbSyntax lowercases display names inside a sentence", "[verbsyntax][casing]") {
    VerbSyntax vs({ "Pick up", "Pick up $target" });
    auto obj = std::make_shared<GameObject>("mug", Vector2<>(0, 0));
    obj->setDisplayName("Mug");

    // Scene object says "Mug", inventory copy says "mug" — both must render
    // the same mid-sentence.
    REQUIRE(vs.getInfo(nullptr, obj) == "Pick up mug");
}

TEST_CASE("VerbSyntax lowercases every word of a multi-word name", "[verbsyntax][casing]") {
    VerbSyntax vs({ "Push", "Push $target" });
    auto obj = std::make_shared<GameObject>("table2", Vector2<>(0, 0));
    obj->setDisplayName("Wooden Table");

    REQUIRE(vs.getInfo(nullptr, obj) == "Push wooden table");
}

TEST_CASE("VerbSyntax capitalizes a name at the start of a sentence", "[verbsyntax][casing]") {
    VerbSyntax vs({ "$target looks heavy" });
    auto obj = std::make_shared<GameObject>("table2", Vector2<>(0, 0));
    obj->setDisplayName("Wooden Table");

    REQUIRE(vs.getInfo(nullptr, obj) == "Wooden table looks heavy");
}

TEST_CASE("A '=' prefix keeps a display name's casing everywhere", "[verbsyntax][casing]") {
    auto robot = std::make_shared<GameObject>("robot", Vector2<>(0, 0));
    robot->setDisplayName("=R.O.B.O.T 3000");

    // The marker is data-only: it never shows up in the name itself.
    REQUIRE(robot->getDisplayName() == "R.O.B.O.T 3000");
    REQUIRE(robot->hasFixedCaseDisplayName());

    VerbSyntax mid({ "Pick up", "Pick up $target" });
    REQUIRE(mid.getInfo(nullptr, robot) == "Pick up R.O.B.O.T 3000");

    VerbSyntax start({ "$target beeps" });
    REQUIRE(start.getInfo(nullptr, robot) == "R.O.B.O.T 3000 beeps");
}

TEST_CASE("Fixed-case names work for proper nouns mid-sentence", "[verbsyntax][casing]") {
    VerbSyntax vs({ "Give", "Give $selected to $target" });
    auto mug = std::make_shared<GameObject>("mug", Vector2<>(0, 0));
    mug->setDisplayName("Mug");
    auto stan = std::make_shared<GameObject>("stan", Vector2<>(0, 0));
    stan->setDisplayName("=Stan");

    REQUIRE(vs.getInfo(mug, stan) == "Give mug to Stan");
}

TEST_CASE("VerbSyntax localizes display names before casing them", "[verbsyntax][casing][localization]") {
    auto& loc = Localization::getInstance();
    loc.setSourceLocale("en");
    loc.addTranslation("es_verbsyntax_test", "Mug", "Taza");
    loc.setLocale("es_verbsyntax_test");

    VerbSyntax vs({ "Pick up", "Pick up $target" });
    auto obj = std::make_shared<GameObject>("mug", Vector2<>(0, 0));
    obj->setDisplayName("Mug");

    // Translated first ("Mug" -> "Taza"), then cased for its position.
    REQUIRE(vs.getInfo(nullptr, obj) == "Pick up taza");

    loc.setLocale("en");
}

TEST_CASE("setDisplayName with explicit casing rule carries names between objects", "[verbsyntax][casing]") {
    auto sceneItem = std::make_shared<GameObject>("robot", Vector2<>(0, 0));
    sceneItem->setDisplayName("=R.O.B.O.T 3000");

    // The scene item -> inventory item handoff (see Item.cpp).
    auto inventoryItem = std::make_shared<GameObject>("robot", Vector2<>(0, 0));
    inventoryItem->setDisplayName(sceneItem->getDisplayName(), sceneItem->hasFixedCaseDisplayName());

    REQUIRE(inventoryItem->getDisplayName() == "R.O.B.O.T 3000");
    REQUIRE(inventoryItem->hasFixedCaseDisplayName());
}
