#include "catch.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "GameObject.hpp"
#include "Verb.hpp"
#include "Event.hpp"
#include "EventInfo.hpp"
#include "Interactable.hpp"
#include "Inventory.hpp"
#include "Item.hpp"
#include "DialogueNode.hpp"
#include "DialogueOption.hpp"
#include "Scene.hpp"
#include "SceneTransitionObject.hpp"

// ---------------------------------------------------------------------------
// Helper: drain any leftover pending events from previous sections
// ---------------------------------------------------------------------------
static void drainPendingEvents() {
    for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

TEST_CASE("Gameplay logic", "[gameplay]") {
    drainPendingEvents();

    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);
    game.setCurrentPlayer(player);

    // -----------------------------------------------------------------------
    // INTERACTABLE — verb registration
    // -----------------------------------------------------------------------
    SECTION("Interactable addVerbInteraction and hasVerbInteraction") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->addVerbInteraction(game.pickupVerb, Event([](EventInfo) {}));
        REQUIRE(obj->hasVerbInteraction(game.pickupVerb) == true);
        REQUIRE(obj->hasVerbInteraction(game.pushVerb) == false);
    }

    SECTION("Interactable removeInteraction removes verb") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->addVerbInteraction(game.pickupVerb, Event([](EventInfo) {}));
        obj->removeInteraction(game.pickupVerb);
        REQUIRE(obj->hasVerbInteraction(game.pickupVerb) == false);
    }

    SECTION("Interactable removeInteraction by name") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->addVerbInteraction(game.pickupVerb, Event([](EventInfo) {}));
        obj->removeInteraction("pickup");
        REQUIRE(obj->hasVerbInteraction(game.pickupVerb) == false);
    }

    // -----------------------------------------------------------------------
    // INTERACTABLE — default verb
    // -----------------------------------------------------------------------
    SECTION("Interactable set and get default verb") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->setDefaultVerb(game.pickupVerb);
        REQUIRE(obj->getDefaultVerb() == game.pickupVerb);
    }

    SECTION("Interactable set default verb by name") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->setDefaultVerb("pickup");
        REQUIRE(obj->getDefaultVerb() == game.pickupVerb);
    }

    SECTION("Interactable default verb is nullptr initially") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE(obj->getDefaultVerb() == nullptr);
    }

    // -----------------------------------------------------------------------
    // INTERACTABLE — interaction resolution
    // -----------------------------------------------------------------------
    SECTION("Interactable interact executes matching verb event") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        bool fired = false;
        obj->addVerbInteraction(game.pickupVerb, Event([&fired](EventInfo) { fired = true; }));
        obj->interact(Vector2<>(5, 5), game.pickupVerb);
        REQUIRE(fired == true);
    }

    SECTION("Interactable interact with non-matching verb fires default event") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        bool defaultFired = false;
        game.pushVerb->setDefaultEvent(Event([&defaultFired](EventInfo) { defaultFired = true; }));
        obj->interact(Vector2<>(5, 5), game.pushVerb);
        REQUIRE(defaultFired == true);
    }

    SECTION("Interactable interact with non-matching verb falls back to any") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        bool anyFired = false;
        obj->addVerbInteraction(game.anyVerb, Event([&anyFired](EventInfo) { anyFired = true; }));
        obj->interact(Vector2<>(5, 5), game.pushVerb);
        REQUIRE(anyFired == true);
    }

    SECTION("Interact with no match consumes the click when the verb has a default response") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        // pushVerb's default says "I can't move that." — that response IS
        // the handling of this click. If it weren't consumed, the click
        // loop would pass the click on to the background, which re-resolves
        // the just-reset current verb (walk) and walks the player while the
        // message is still up.
        bool result = obj->interact(Vector2<>(5, 5), game.pushVerb);
        REQUIRE(result == true);
    }

    SECTION("Interact with no match resets the verb so the NEXT click walks") {
        auto previousDefault = game.getDefaultVerb();
        game.setDefaultVerb(game.walkVerb);

        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        player->setVerb(game.pushVerb);
        obj->interact(Vector2<>(5, 5), game.pushVerb);
        REQUIRE(player->getVerb() == game.walkVerb);

        game.setDefaultVerb(previousDefault);
    }

    SECTION("Interact with no match and a truly empty default does not consume the click") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        auto silentVerb = std::make_shared<Verb>("silent");
        silentVerb->setDefaultEvent(Event());
        bool result = obj->interact(Vector2<>(5, 5), silentVerb);
        REQUIRE(result == false);
    }

    // -----------------------------------------------------------------------
    // INTERACTABLE — before/after event order
    // -----------------------------------------------------------------------
    SECTION("Interaction executes before, interaction, and after events in order") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        std::vector<int> order;
        game.pickupVerb->setBeforeActualEvent(Event([&order](EventInfo) { order.push_back(1); }));
        obj->addVerbInteraction(game.pickupVerb, Event([&order](EventInfo) { order.push_back(2); }));
        game.pickupVerb->setAfterActualEvent(Event([&order](EventInfo) { order.push_back(3); }));
        obj->interact(Vector2<>(5, 5), game.pickupVerb);
        REQUIRE(order.size() == 3);
        REQUIRE(order[0] == 1);
        REQUIRE(order[1] == 2);
        REQUIRE(order[2] == 3);
    }

    // -----------------------------------------------------------------------
    // INTERACTABLE — requiresSelectedObject flow
    // -----------------------------------------------------------------------
    SECTION("interact with requiresSelectedObject selects object first") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->addVerbInteraction(game.giveVerb, Event([](EventInfo) {}));
        // First call — no selected object, should select this object
        obj->interact(Vector2<>(5, 5), game.giveVerb);
        REQUIRE(player->getSelectedObject() == obj);
    }

    SECTION("interact with requiresSelectedObject and correct name fires event") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        bool fired = false;
        obj->addVerbInteraction(game.giveVerb, "box", Event([&fired](EventInfo) { fired = true; }));
        // First call selects the object
        obj->interact(Vector2<>(5, 5), game.giveVerb);
        REQUIRE(player->getSelectedObject() == obj);
        // Second call should fire the event (selected object name matches)
        player->setSelectedObject(obj);
        obj->interact(Vector2<>(5, 5), game.giveVerb);
        REQUIRE(fired == true);
    }

    SECTION("interact with requiresSelectedObject and wrong name does not fire") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(10, 10));
        auto box = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        bool fired = false;
        box->addVerbInteraction(game.giveVerb, "box", Event([&fired](EventInfo) { fired = true; }));
        // Select apple, then interact with box — names don't match
        player->setSelectedObject(apple);
        box->interact(Vector2<>(5, 5), game.giveVerb);
        REQUIRE(fired == false);
    }

    // -----------------------------------------------------------------------
    // COMBINATIONS — useVerb combination registry
    // -----------------------------------------------------------------------
    SECTION("combination registry fires when matching pair used") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(5, 5));
        auto key = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        bool fired = false;
        game.addCombination("apple", "key", Event([&fired](EventInfo) { fired = true; }));
        player->setSelectedObject(apple);
        key->interact(Vector2<>(5, 5), game.useVerb);
        REQUIRE(fired == true);
    }

    SECTION("combination registry does not fire for wrong target") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(5, 5));
        auto key = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        auto rock = std::make_shared<GameObject>("rock", Vector2<>(0, 0), Vector2<>(5, 5));
        bool fired = false;
        game.addCombination("apple", "key", Event([&fired](EventInfo) { fired = true; }));
        player->setSelectedObject(apple);
        rock->interact(Vector2<>(5, 5), game.useVerb);
        REQUIRE(fired == false);
    }

    SECTION("combination registry does not fire for wrong selected item") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(5, 5));
        auto key = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        bool fired = false;
        game.addCombination("apple", "key", Event([&fired](EventInfo) { fired = true; }));
        player->setSelectedObject(key);
        key->interact(Vector2<>(5, 5), game.useVerb);
        REQUIRE(fired == false);
    }

    SECTION("combination registry fires and clears selected object") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(5, 5));
        auto key = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        bool fired = false;
        game.addCombination("apple", "key", Event([&fired](EventInfo) { fired = true; }));
        player->setSelectedObject(apple);
        key->interact(Vector2<>(5, 5), game.useVerb);
        REQUIRE(fired == true);
        REQUIRE(player->getSelectedObject() == nullptr);
    }

    SECTION("per-object useVerb interaction takes priority over combination") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(5, 5));
        auto key = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        bool perObjFired = false;
        bool comboFired = false;
        key->addVerbInteraction(game.useVerb, "apple", Event([&perObjFired](EventInfo) { perObjFired = true; }));
        game.addCombination("apple", "key", Event([&comboFired](EventInfo) { comboFired = true; }));
        player->setSelectedObject(apple);
        key->interact(Vector2<>(5, 5), game.useVerb);
        REQUIRE(perObjFired == true);
        REQUIRE(comboFired == false);
    }

    // -----------------------------------------------------------------------
    // EVENT — execution with callbacks
    // -----------------------------------------------------------------------
    SECTION("Event execute fires void lambda") {
        bool fired = false;
        Event e([&fired]() { fired = true; });
        e.execute();
        REQUIRE(fired == true);
    }

    SECTION("Event execute fires EventInfo lambda") {
        bool fired = false;
        Event e([&fired](EventInfo info) { fired = true; });
        e.execute();
        REQUIRE(fired == true);
    }

    SECTION("Event execute passes EventInfo with EVENT_NOT_TRIGGERED_BY_CLICK") {
        Vector2<> clickPos(0, 0);
        Event e([&clickPos](EventInfo info) { clickPos = info.clickPos; });
        e.execute();
        REQUIRE(std::isinf(clickPos.x));
        REQUIRE(clickPos.x < 0);
        REQUIRE(std::isinf(clickPos.y));
        REQUIRE(clickPos.y < 0);
    }

    SECTION("Event execute runs multiple actions in sequence") {
        int count = 0;
        Event e(std::vector<Event::Action>{
            [&count]() { count++; },
            [&count]() { count++; },
            [&count]() { count++; }
        });
        e.execute();
        REQUIRE(count == 3);
    }

    // -----------------------------------------------------------------------
    // EVENT — Delay scheduling
    // -----------------------------------------------------------------------
    SECTION("Event with Delay fires scheduled actions after tick") {
        int fired = 0;
        Event e({
            [&fired]() { fired++; },
            Delay(1.0f),
            [&fired]() { fired++; }
        });
        e.execute();
        REQUIRE(fired == 1);
        Event::tick(2.0f);
        REQUIRE(fired == 2);
    }

    SECTION("Event with Delay does not fire before delay expires") {
        int fired = 0;
        Event e({
            Delay(5.0f),
            [&fired]() { fired++; }
        });
        e.execute();
        REQUIRE(fired == 0);
        Event::tick(2.0f);
        REQUIRE(fired == 0);
        Event::tick(4.0f);
        REQUIRE(fired == 1);
    }

    SECTION("Event with multiple Delays chains correctly") {
        std::vector<int> order;
        Event e({
            [&order]() { order.push_back(1); },
            Delay(0.1f),
            [&order]() { order.push_back(2); },
            Delay(0.1f),
            [&order]() { order.push_back(3); }
        });
        e.execute();
        REQUIRE(order.size() == 1);
        REQUIRE(order[0] == 1);
        Event::tick(0.2f);
        REQUIRE(order.size() == 2);
        REQUIRE(order[1] == 2);
        Event::tick(0.2f);
        REQUIRE(order.size() == 3);
        REQUIRE(order[2] == 3);
    }

    // -----------------------------------------------------------------------
    // DIALOGUE — integration
    // -----------------------------------------------------------------------
    SECTION("DialogueNode enterNode fires its event") {
        bool fired = false;
        auto node = std::make_shared<DialogueNode>(Event([&fired](EventInfo) { fired = true; }));
        node->enterNode();
        REQUIRE(fired == true);
    }

    SECTION("DialogueOption select enters next node") {
        bool nextFired = false;
        auto nodeB = std::make_shared<DialogueNode>(Event([&nextFired](EventInfo) { nextFired = true; }));
        auto option = std::make_shared<DialogueOption>("Go to B", nodeB);
        option->select();
        REQUIRE(nextFired == true);
    }

    SECTION("DialogueOption select with no next node does not crash") {
        auto option = std::make_shared<DialogueOption>("End");
        REQUIRE_NOTHROW(option->select());
    }

    SECTION("Dialogue chain A -> B -> C fires all events in order") {
        std::vector<int> order;
        auto nodeC = std::make_shared<DialogueNode>(Event([&order](EventInfo) { order.push_back(3); }));
        auto optC = std::make_shared<DialogueOption>("End", nodeC);

        auto nodeB = std::make_shared<DialogueNode>(Event([&order](EventInfo) { order.push_back(2); }));
        nodeB->setOptions({ optC });

        auto optionB = std::make_shared<DialogueOption>("Go to B", nodeB);

        auto nodeA = std::make_shared<DialogueNode>(Event([&order](EventInfo) { order.push_back(1); }));
        nodeA->setOptions({ optionB });

        nodeA->enterNode();
        REQUIRE(order.size() == 1);
        REQUIRE(order[0] == 1);

        optionB->select();
        REQUIRE(order.size() == 2);
        REQUIRE(order[1] == 2);

        optC->select();
        REQUIRE(order.size() == 3);
        REQUIRE(order[2] == 3);
    }

    SECTION("DialogueNode showOptions with options creates UI elements") {
        auto node = std::make_shared<DialogueNode>(Event([] {}));
        node->setOptions({
            std::make_shared<DialogueOption>("Opt 1"),
            std::make_shared<DialogueOption>("Opt 2")
        });

        game.getUI().createLayout("dialogue");
        game.getUI().setLayout("dialogue");
        REQUIRE_NOTHROW(node->showOptions());
    }

    SECTION("DialogueNode showOptions with no options re-enables movement") {
        auto node = std::make_shared<DialogueNode>(Event([] {}));

        game.getUI().createLayout("dialogue");
        game.getUI().createLayout("afterDialogue");
        game.getUI().setDialogueLayout(&game.getUI().getLayout("dialogue"));
        game.getUI().setAfterDialogueLayout(&game.getUI().getLayout("afterDialogue"));
        game.getUI().setLayout("dialogue");

        player->disableMovement();
        node->showOptions();
        REQUIRE(player->canMove() == true);
    }

    // -----------------------------------------------------------------------
    // HOVER TARGET — the object named in the verb sentence line
    // -----------------------------------------------------------------------
    SECTION("Hovering a scene object targets it even without a default verb") {
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        // An NPC like Stan: interactions but no default verb.
        auto npc = std::make_shared<GameObject>("hover_npc", Vector2<>(0, 0), Vector2<>(10, 10));
        npc->addVerbInteraction(game.pickupVerb, Event([](EventInfo) {}));
        scene->addGameObject(npc);

        npc->onMouseOver(Vector2<>(5, 5));
        REQUIRE(player->getTargetObject() == npc);

        npc->onMouseNotOver(Vector2<>(50, 50));
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("Hovering an object with no interactions at all still targets it") {
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        auto prop = std::make_shared<GameObject>("hover_prop", Vector2<>(0, 0), Vector2<>(10, 10));
        scene->addGameObject(prop);

        prop->onMouseOver(Vector2<>(5, 5));
        REQUIRE(player->getTargetObject() == prop);

        prop->onMouseNotOver(Vector2<>(50, 50));
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("Hovering scenery marked non-interactable does not target it") {
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        auto decoration = std::make_shared<GameObject>("hover_decoration", Vector2<>(0, 0), Vector2<>(10, 10));
        decoration->setInteractable(false);
        scene->addGameObject(decoration);

        decoration->onMouseOver(Vector2<>(5, 5));
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("Scene backgrounds are scenery: hovering them does not target them") {
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        auto background = scene->addBackground("missing_bg.png", Vector2<int>(640, 144));
        REQUIRE_FALSE(background->isInteractable());

        background->onMouseOver(Vector2<>(100, 100));
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("Hovering an object outside any scene does not target it") {
        auto floating = std::make_shared<GameObject>("hover_floating", Vector2<>(0, 0), Vector2<>(10, 10));

        floating->onMouseOver(Vector2<>(5, 5));
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("Hovering the current player does not target them") {
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);
        scene->addGameObject(player);

        player->onMouseOver(Vector2<>(5, 5));
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("Leaving one object does not clear a target set by another") {
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        auto first = std::make_shared<GameObject>("hover_first", Vector2<>(0, 0), Vector2<>(10, 10));
        auto second = std::make_shared<GameObject>("hover_second", Vector2<>(10, 0), Vector2<>(10, 10));
        scene->addGameObject(first);
        scene->addGameObject(second);

        // Pointer slides from first onto second; depending on iteration order
        // second's mouse-over can fire before first's mouse-not-over.
        first->onMouseOver(Vector2<>(5, 5));
        second->onMouseOver(Vector2<>(15, 5));
        first->onMouseNotOver(Vector2<>(15, 5));
        REQUIRE(player->getTargetObject() == second);
    }

    // -----------------------------------------------------------------------
    // MOUSE PRESENCE — invisible objects vs absent objects
    // -----------------------------------------------------------------------
    SECTION("An invisible but interactable object still takes clicks and hovers") {
        // A closed door whose art is part of the background: the object
        // renders nothing but is very much there.
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        auto door = std::make_shared<GameObject>("hidden_door", Vector2<>(0, 0), Vector2<>(10, 10));
        bool opened = false;
        door->addVerbInteraction(game.pushVerb, Event([&opened](EventInfo) { opened = true; }));
        door->setVisible(false);
        scene->addGameObject(door);

        auto& input = game.getInputManager();
        REQUIRE(input.checkObjectClicked(5, 5, door, game.pushVerb));
        REQUIRE(opened);

        input.checkMouseOverObject(5, 5, door);
        REQUIRE(player->getTargetObject() == door);
        input.checkMouseOverObject(50, 50, door);
        REQUIRE(player->getTargetObject() == nullptr);
    }

    SECTION("An invisible AND non-interactable object is absent to the mouse") {
        // A picked-up item: hidden and inert until a load restores it.
        auto scene = std::make_shared<Scene>();
        game.changeScene(scene, 0.0f);

        auto taken = std::make_shared<GameObject>("taken_item", Vector2<>(0, 0), Vector2<>(10, 10));
        bool fired = false;
        taken->addVerbInteraction(game.pickupVerb, Event([&fired](EventInfo) { fired = true; }));
        taken->setVisible(false);
        taken->setInteractable(false);
        scene->addGameObject(taken);

        auto& input = game.getInputManager();
        REQUIRE_FALSE(input.checkObjectClicked(5, 5, taken, game.pickupVerb));
        REQUIRE_FALSE(fired);

        input.checkMouseOverObject(5, 5, taken);
        REQUIRE(player->getTargetObject() == nullptr);
    }

    // -----------------------------------------------------------------------
    // PLAYER
    // -----------------------------------------------------------------------
    SECTION("Player canMove starts enabled") {
        REQUIRE(player->canMove() == true);
    }

    SECTION("Player disableMovement and enableMovement") {
        player->disableMovement();
        REQUIRE(player->canMove() == false);
        player->enableMovement();
        REQUIRE(player->canMove() == true);
    }

    SECTION("Player setVerb by name") {
        player->setVerb("pickup");
        REQUIRE(player->getVerb() == game.pickupVerb);
    }

    SECTION("Player setVerb by shared_ptr") {
        player->setVerb(game.pushVerb);
        REQUIRE(player->getVerb() == game.pushVerb);
    }

    SECTION("Player setVerb with unknown name returns nullptr") {
        player->setVerb("nonexistent");
        REQUIRE(player->getVerb() == nullptr);
    }

    SECTION("Player selectedObject is initially null") {
        REQUIRE(player->getSelectedObject() == nullptr);
    }

    SECTION("Player setSelectedObject and getSelectedObject") {
        auto obj = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        player->setSelectedObject(obj);
        REQUIRE(player->getSelectedObject() == obj);
        player->setSelectedObject(nullptr);
        REQUIRE(player->getSelectedObject() == nullptr);
    }

    SECTION("Player selectedObject uses weak_ptr — freed object returns null") {
        auto obj = std::make_shared<GameObject>("key", Vector2<>(0, 0), Vector2<>(5, 5));
        player->setSelectedObject(obj);
        obj.reset();
        REQUIRE(player->getSelectedObject() == nullptr);
    }

    SECTION("Player inventory access") {
        Inventory& inv = player->getInventory();
        REQUIRE(inv.getItems().empty());
        auto item = std::make_shared<GameObject>("coin", Vector2<>(0, 0), Vector2<>(5, 5));
        inv.addItem(item);
        REQUIRE(inv.getItems().size() == 1);
    }

    // -----------------------------------------------------------------------
    // GAMEOBJECT
    // -----------------------------------------------------------------------
    SECTION("GameObject addTag and hasTag") {
        auto obj = std::make_shared<GameObject>("item", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE(obj->hasTag("item") == false);
        obj->addTag("item");
        REQUIRE(obj->hasTag("item") == true);
        REQUIRE(obj->hasTag("weapon") == false);
    }

    SECTION("GameObject setState and getState") {
        auto obj = std::make_shared<GameObject>("door", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE(obj->getState() == 0);
        obj->setState(2);
        REQUIRE(obj->getState() == 2);
    }

    SECTION("GameObject visibility starts visible") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE(obj->isVisible() == true);
    }

    SECTION("GameObject toggleVisibility") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(0, 0), Vector2<>(10, 10));
        obj->toggleVisibility();
        REQUIRE(obj->isVisible() == false);
        obj->toggleVisibility();
        REQUIRE(obj->isVisible() == true);
    }

    SECTION("GameObject position getters and setters") {
        auto obj = std::make_shared<GameObject>("box", Vector2<>(15, 25), Vector2<>(30, 40));
        REQUIRE(obj->getPosition().x == 15.0f);
        REQUIRE(obj->getPosition().y == 25.0f);
        REQUIRE(obj->getSize().x == 30.0f);
        REQUIRE(obj->getSize().y == 40.0f);
        obj->setPosition(Vector2<>(100, 200));
        REQUIRE(obj->getPosition().x == 100.0f);
    }

    // -----------------------------------------------------------------------
    // INTEGRATION — full interaction flows
    // -----------------------------------------------------------------------
    SECTION("Integration: pickup interaction fires and adds item to inventory") {
        auto obj = std::make_shared<GameObject>("gem", Vector2<>(100, 100), Vector2<>(10, 10));

        bool pickupFired = false;
        obj->addVerbInteraction(game.pickupVerb, Event([&pickupFired, &player](EventInfo e) {
            pickupFired = true;
            auto item = std::make_shared<GameObject>("gem", Vector2<>(0, 0), Vector2<>(5, 5));
            item->addTag("item");
            player->getInventory().addItem(item);
        }));

        obj->interact(Vector2<>(105, 105), game.pickupVerb);
        REQUIRE(pickupFired == true);
        REQUIRE(player->getInventory().getItems().size() == 1);
        REQUIRE(player->getInventory().getItemAt(0)->getName() == "gem");
    }

    SECTION("Integration: give verb flow — select item then give to target") {
        auto apple = std::make_shared<GameObject>("apple", Vector2<>(0, 0), Vector2<>(5, 5));
        apple->addTag("item");
        player->getInventory().addItem(apple);

        auto npc = std::make_shared<GameObject>("npc", Vector2<>(200, 200), Vector2<>(20, 20));
        bool giveFired = false;
        npc->addVerbInteraction(game.giveVerb, "apple", Event([&giveFired](EventInfo e) {
            giveFired = true;
        }));

        // Select the apple
        player->setSelectedObject(apple);
        REQUIRE(player->getSelectedObject() == apple);

        // Give apple to npc
        npc->interact(Vector2<>(205, 205), game.giveVerb);
        REQUIRE(giveFired == true);
    }

    // -----------------------------------------------------------------------
    // VERB — Game verb lookup
    // -----------------------------------------------------------------------
    SECTION("Game standard verbs are created") {
        REQUIRE(game.walkVerb != nullptr);
        REQUIRE(game.pushVerb != nullptr);
        REQUIRE(game.closeVerb != nullptr);
        REQUIRE(game.openVerb != nullptr);
        REQUIRE(game.talkVerb != nullptr);
        REQUIRE(game.giveVerb != nullptr);
        REQUIRE(game.pickupVerb != nullptr);
        REQUIRE(game.useVerb != nullptr);
        REQUIRE(game.lookAtVerb != nullptr);
        REQUIRE(game.pullVerb != nullptr);
        REQUIRE(game.anyVerb != nullptr);
    }

    SECTION("Game getVerb returns verbs by name") {
        REQUIRE(game.getVerb("walk") == game.walkVerb);
        REQUIRE(game.getVerb("push") == game.pushVerb);
        REQUIRE(game.getVerb("give") == game.giveVerb);
        REQUIRE(game.getVerb("nonexistent") == nullptr);
    }

    SECTION("Give and use verbs require selected object") {
        REQUIRE(game.giveVerb->RequiresSelectedObject() == true);
        REQUIRE(game.useVerb->RequiresSelectedObject() == true);
        REQUIRE(game.walkVerb->RequiresSelectedObject() == false);
    }

    // -----------------------------------------------------------------------
    // SCENE TRANSITIONS (Phase 3a)
    // -----------------------------------------------------------------------
    SECTION("SceneTransitionObject walk interaction changes scene") {
        auto sceneA = std::make_shared<Scene>();
        auto sceneB = std::make_shared<Scene>();

        player->moveToScene(sceneA);
        game.changeScene(sceneA);
        REQUIRE(game.getScene() == sceneA);
        REQUIRE(player->getScene() == sceneA);

        auto transition = std::make_shared<SceneTransitionObject>(
            "door_to_B", Vector2<>(0, 0), Vector2<>(10, 10), sceneB
        );

        transition->interact(Vector2<>(5, 5), game.walkVerb);
        REQUIRE(game.getScene() == sceneB);
        REQUIRE(player->getScene() == sceneB);
    }

    // -----------------------------------------------------------------------
    // DEFAULT VERB EVENTS (Phase 3b)
    // -----------------------------------------------------------------------
    SECTION("Default verb event fires for lookAt when no interaction exists") {
        auto obj = std::make_shared<GameObject>("rock", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE_NOTHROW(obj->interact(Vector2<>(5, 5), game.lookAtVerb));
    }

    SECTION("Default verb event fires for push when no interaction exists") {
        auto obj = std::make_shared<GameObject>("boulder", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE_NOTHROW(obj->interact(Vector2<>(5, 5), game.pushVerb));
    }

    SECTION("Default verb event fires for pull when no interaction exists") {
        auto obj = std::make_shared<GameObject>("lever", Vector2<>(0, 0), Vector2<>(10, 10));
        REQUIRE_NOTHROW(obj->interact(Vector2<>(5, 5), game.pullVerb));
    }
}
