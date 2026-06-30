#include "catch.hpp"
#include "Inventory.hpp"
#include "GameObject.hpp"

TEST_CASE("Inventory is initially empty", "[inventory]") {
    Inventory inv;
    REQUIRE(inv.getItems().empty());
    REQUIRE(inv.getItemAt(0) == nullptr);
}

TEST_CASE("Inventory addItem", "[inventory]") {
    Inventory inv;
    auto item = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    inv.addItem(item);
    REQUIRE(inv.getItems().size() == 1);
    REQUIRE(inv.getItemAt(0) == item);
}

TEST_CASE("Inventory addItem multiple items", "[inventory]") {
    Inventory inv;
    auto key = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    auto coin = std::make_shared<GameObject>("coin", Vector2<>(0, 0));
    inv.addItem(key);
    inv.addItem(coin);
    REQUIRE(inv.getItems().size() == 2);
    REQUIRE(inv.getItemAt(0)->getName() == "key");
    REQUIRE(inv.getItemAt(1)->getName() == "coin");
}

TEST_CASE("Inventory removeItem removes existing item", "[inventory]") {
    Inventory inv;
    auto item = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    inv.addItem(item);
    inv.removeItem(item);
    REQUIRE(inv.getItems().empty());
}

TEST_CASE("Inventory removeItem does nothing for unknown item", "[inventory]") {
    Inventory inv;
    auto itemA = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    auto itemB = std::make_shared<GameObject>("coin", Vector2<>(0, 0));
    inv.addItem(itemA);
    inv.removeItem(itemB);
    REQUIRE(inv.getItems().size() == 1);
}

TEST_CASE("Inventory removeItem preserves order of remaining items", "[inventory]") {
    Inventory inv;
    auto a = std::make_shared<GameObject>("a", Vector2<>(0, 0));
    auto b = std::make_shared<GameObject>("b", Vector2<>(0, 0));
    auto c = std::make_shared<GameObject>("c", Vector2<>(0, 0));
    inv.addItem(a);
    inv.addItem(b);
    inv.addItem(c);
    inv.removeItem(b);
    REQUIRE(inv.getItems().size() == 2);
    REQUIRE(inv.getItemAt(0)->getName() == "a");
    REQUIRE(inv.getItemAt(1)->getName() == "c");
}

TEST_CASE("Inventory getItemAt with negative index returns nullptr", "[inventory]") {
    Inventory inv;
    auto item = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    inv.addItem(item);
    REQUIRE(inv.getItemAt(-1) == nullptr);
}

TEST_CASE("Inventory getItemAt with out-of-bounds index returns nullptr", "[inventory]") {
    Inventory inv;
    auto item = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    inv.addItem(item);
    REQUIRE(inv.getItemAt(5) == nullptr);
}

TEST_CASE("Inventory getItems returns modifiable reference", "[inventory]") {
    Inventory inv;
    auto item = std::make_shared<GameObject>("key", Vector2<>(0, 0));
    inv.addItem(item);
    auto& items = inv.getItems();
    REQUIRE(items.size() == 1);
    items.clear();
    REQUIRE(inv.getItems().empty());
}
