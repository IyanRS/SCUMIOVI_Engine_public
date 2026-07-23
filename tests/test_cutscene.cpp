#include "catch.hpp"
#include "Cutscene.hpp"
#include "CutsceneLoader.hpp"
#include "Game.hpp"
#include "Player.hpp"
#include "Scene.hpp"
#include "Camera.hpp"
#include <fstream>
#include <cstdio>

static void drainPendingEvents() {
    for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

namespace {
    void writeFile(const std::string& path, const std::string& contents) {
        std::ofstream f(path);
        f << contents;
    }

    // Event::execute() (the no-EventInfo overload every async step's
    // completion callback eventually goes through — moveTo, camera moveTo,
    // fades, say) is a no-op if Game has no current player set. Game is a
    // singleton shared by the whole test binary, so rather than rely on
    // whatever an earlier, unrelated test left it as, every test here makes
    // its own current player explicit.
    std::shared_ptr<Player> ensureCurrentPlayer() {
        static int counter = 0;
        Game& game = Game::getInstance();
        auto p = std::make_shared<Player>("cutscene_ambient_player_" + std::to_string(counter++), Vector2<>(0, 0), Vector2<>(10, 10));
        game.addPlayer(p);
        game.setCurrentPlayer(p);
        return p;
    }
}

TEST_CASE("Cutscene runs steps in order", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    std::vector<int> order;

    Event cs = Cutscene()
        .run([&order]() { order.push_back(1); })
        .run([&order]() { order.push_back(2); })
        .run([&order]() { order.push_back(3); })
        .build();

    cs.execute();
    REQUIRE(order == std::vector<int>{1, 2, 3});
}

TEST_CASE("Cutscene::wait delays continuation until the duration elapses", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    bool ran = false;

    Event cs = Cutscene()
        .wait(1.0f)
        .run([&ran]() { ran = true; })
        .build();

    cs.execute();
    REQUIRE_FALSE(ran);

    Event::tick(0.5f);
    REQUIRE_FALSE(ran);

    Event::tick(0.6f);
    REQUIRE(ran);
}

TEST_CASE("Cutscene::moveCamera moves the camera and continues on completion", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Camera camera;
    bool continued = false;

    Event cs = Cutscene()
        .moveCamera(camera, Vector2<>(50, 60), 1.0f)
        .run([&continued]() { continued = true; })
        .build();

    cs.execute();
    REQUIRE_FALSE(continued);

    camera.update(1.1f);
    REQUIRE(continued);
    REQUIRE(camera.getPosition().x == 50);
    REQUIRE(camera.getPosition().y == 60);
}

TEST_CASE("Cutscene::walk moves an object and continues on completion", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    auto obj = std::make_shared<GameObject>("cutscene_walk_obj", Vector2<>(0, 0), Vector2<>(10, 10));
    bool continued = false;

    Event cs = Cutscene()
        .walk(obj, Vector2<>(20, 30), 1.0f)
        .run([&continued]() { continued = true; })
        .build();

    cs.execute();
    obj->update(1.1f);
    REQUIRE(continued);
    REQUIRE(obj->getPosition().x == 20);
    REQUIRE(obj->getPosition().y == 30);
}

TEST_CASE("Cutscene::say sets the object's displayed text", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    auto obj = std::make_shared<GameObject>("cutscene_say_obj", Vector2<>(0, 0), Vector2<>(10, 10));

    Event cs = Cutscene()
        .say(obj, "Hello", 1.0f)
        .build();

    cs.execute();
    REQUIRE(obj->getTextOver().toString() == "Hello");

    // Let the say run out so the cutscene actually finishes — a playback
    // left hanging would keep Cutscene::isPlaying() true for later tests.
    obj->update(1.1f);
}

TEST_CASE("Cutscene::changeScene switches the active scene and continues once done", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();

    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();
    game.changeScene(sceneA, 0.0f);
    REQUIRE(game.getScene() == sceneA);

    bool continued = false;
    Event cs = Cutscene()
        .changeScene(sceneB, 0.0f)
        .run([&continued]() { continued = true; })
        .build();

    cs.execute();
    REQUIRE(game.getScene() == sceneB);
    REQUIRE(continued);
}

TEST_CASE("Cutscene::changeScene with a fade delays continuation until the fade completes", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();
    auto& r = game.getRenderer();

    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();
    game.changeScene(sceneA, 0.0f);

    bool continued = false;
    Event cs = Cutscene()
        .changeScene(sceneB, 0.5f)
        .run([&continued]() { continued = true; })
        .build();

    cs.execute();
    REQUIRE(game.getScene() == sceneA); // not yet, still fading
    REQUIRE_FALSE(continued);

    for (int i = 0; i < 100; i++) r.updateFade(0.1f);
    REQUIRE(game.getScene() == sceneB);
    REQUIRE(continued);
}

TEST_CASE("Cutscene::fadeToBlack / fadeFromBlack integrate with the renderer", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    auto& r = Game::getInstance().getRenderer();
    bool continued = false;

    Event cs = Cutscene()
        .fadeToBlack(0.5f)
        .fadeFromBlack(0.5f)
        .run([&continued]() { continued = true; })
        .build();

    cs.execute();
    REQUIRE(r.isFading());
    for (int i = 0; i < 100 && !continued; i++) r.updateFade(0.1f);
    REQUIRE(continued);
    REQUIRE_FALSE(r.isFading());
}

TEST_CASE("Cutscene::parallel waits for every branch before continuing", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    bool shortDone = false;
    bool longDone = false;
    bool continued = false;

    Event cs = Cutscene()
        .parallel({
            Cutscene().wait(0.2f).run([&shortDone]() { shortDone = true; }),
            Cutscene().wait(0.5f).run([&longDone]() { longDone = true; })
        })
        .run([&continued]() { continued = true; })
        .build();

    cs.execute();
    Event::tick(0.3f);
    REQUIRE(shortDone);
    REQUIRE_FALSE(longDone);
    REQUIRE_FALSE(continued); // still waiting on the longer branch

    Event::tick(0.3f);
    REQUIRE(longDone);
    REQUIRE(continued);
}

TEST_CASE("Cutscene::disableMovementFor disables movement during playback and re-enables it after", "[cutscene]") {
    drainPendingEvents();
    auto player = ensureCurrentPlayer();
    REQUIRE(player->canMove());

    Event cs = Cutscene()
        .disableMovementFor(player)
        .wait(0.5f)
        .build();

    cs.execute();
    REQUIRE_FALSE(player->canMove());

    Event::tick(0.6f);
    REQUIRE(player->canMove());
}

TEST_CASE("Cutscene::requestSkip before playback jumps straight to the end", "[cutscene]") {
    drainPendingEvents();
    auto player = ensureCurrentPlayer();
    std::vector<int> order;

    Event cs = Cutscene()
        .disableMovementFor(player)
        .run([&order]() { order.push_back(1); })
        .run([&order]() { order.push_back(2); })
        .build();

    Cutscene::requestSkip();
    cs.execute();

    REQUIRE(order.empty());       // nothing ran — skip fired at the very first boundary
    REQUIRE(player->canMove());   // re-enabled by the skip's onDone
}

TEST_CASE("Cutscene::requestSkip lets the in-progress step finish but skips what follows", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    std::vector<int> order;

    Event cs = Cutscene()
        .run([&order]() { order.push_back(1); })
        .wait(1.0f)
        .run([&order]() { order.push_back(2); })
        .wait(1.0f)
        .run([&order]() { order.push_back(3); })
        .build();

    cs.execute();
    REQUIRE(order == std::vector<int>{1});

    Event::tick(0.5f);
    Cutscene::requestSkip(); // requested mid-wait: doesn't cut the wait short...
    Event::tick(0.6f);       // ...but the wait finishing reaches the next boundary, which skips the rest
    REQUIRE(order == std::vector<int>{1});

    Event::tick(2.0f); // nothing left to run even after plenty more time
    REQUIRE(order == std::vector<int>{1});
}

TEST_CASE("Cutscene skip flag does not leak into a later cutscene", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Cutscene::requestSkip();
    Cutscene().run([]() {}).build().execute(); // consumes the pending skip

    std::vector<int> order;
    Event cs = Cutscene().run([&order]() { order.push_back(1); }).build();
    cs.execute();
    REQUIRE(order == std::vector<int>{1}); // not skipped this time
}

TEST_CASE("A walk-then-say chain runs to completion in order", "[cutscene]") {
    drainPendingEvents();
    auto player = ensureCurrentPlayer();
    player->setPosition(Vector2<>(0, 0));

    Event cs = Cutscene()
        .disableMovementFor(player)
        .walk(player, Vector2<>(100, 100), 2.0f)
        .say(player, "First line")
        .say(player, "Second line")
        .build();

    cs.execute();
    REQUIRE_FALSE(player->canMove());

    // The walk: nothing said yet.
    for (int i = 0; i < 41; i++) player->update(0.05f); // ~2.05s
    REQUIRE(player->getPosition().x == 100);
    REQUIRE(player->getPosition().y == 100);
    REQUIRE(player->getTextOver().toString() == "First line");

    // "First line" is 10 chars -> 1.0s; run it out and the next one starts.
    for (int i = 0; i < 21; i++) player->update(0.05f);
    REQUIRE(player->getTextOver().toString() == "Second line");

    // Run well past the end; movement coming back is the chain's own signal
    // that every step finished.
    for (int i = 0; i < 100 && !player->canMove(); i++) player->update(0.05f);
    REQUIRE(player->canMove());
}

TEST_CASE("A click-driven walk can't hijack a cutscene's pending callback", "[cutscene]") {
    drainPendingEvents();
    auto player = ensureCurrentPlayer();
    player->setPosition(Vector2<>(0, 0));

    Event cs = Cutscene()
        .disableMovementFor(player)
        .walk(player, Vector2<>(100, 0), 1.0f)
        .say(player, "Made it")
        .build();

    cs.execute();

    // Mid-walk, a stray click-to-walk arrives. Game code guards on
    // canMove() (see ScriptLoader's walk event), and the cutscene has it
    // disabled — without that guard this moveToAtSpeed would overwrite the
    // pending move callback and the rest of the chain would never run.
    for (int i = 0; i < 10; i++) player->update(0.05f);
    if (player->canMove()) {
        player->moveToAtSpeed(Vector2<>(300, 0), 100.0f);
    }

    for (int i = 0; i < 15; i++) player->update(0.05f);
    REQUIRE(player->getPosition().x == 100);
    REQUIRE(player->getTextOver().toString() == "Made it");

    // Let it finish so the playback doesn't leak into later tests.
    for (int i = 0; i < 100 && !player->canMove(); i++) player->update(0.05f);
}

TEST_CASE("Cutscene::isPlaying is true exactly while a cutscene runs", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    REQUIRE_FALSE(Cutscene::isPlaying());

    Event cs = Cutscene().wait(1.0f).build();
    cs.execute();
    REQUIRE(Cutscene::isPlaying());

    Event::tick(0.5f);
    REQUIRE(Cutscene::isPlaying());

    Event::tick(0.6f);
    REQUIRE_FALSE(Cutscene::isPlaying());
}

TEST_CASE("Cutscene::isPlaying clears when a cutscene is skipped", "[cutscene]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Event cs = Cutscene().wait(1.0f).wait(1.0f).build();
    cs.execute();
    REQUIRE(Cutscene::isPlaying());

    Cutscene::requestSkip();
    Event::tick(1.1f); // the in-flight wait finishes; the skip fires at the boundary
    REQUIRE_FALSE(Cutscene::isPlaying());
}

// --- CutsceneLoader ---------------------------------------------------------

TEST_CASE("CutsceneLoader builds a linear cutscene from JSON", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();

    auto scene = std::make_shared<Scene>();
    game.changeScene(scene, 0.0f);

    auto actor = std::make_shared<GameObject>("loader_actor", Vector2<>(0, 0), Vector2<>(10, 10));
    scene->addGameObject(actor);

    const char* path = "test_cutscene_basic.json";
    writeFile(path, R"({
        "name": "loader_basic",
        "steps": [
            { "type": "walk", "object": "loader_actor", "position": [15, 25], "seconds": 0.5 },
            { "type": "say", "object": "loader_actor", "text": "Hi there", "seconds": 0.5 }
        ]
    })");

    Event cs = CutsceneLoader::loadCutscene(path);
    cs.execute();
    // Small increments: a single dt big enough to finish the 0.5s walk would
    // also immediately expire the say() it triggers (both movement.update and
    // text.update run within the same GameObject::update call using the same
    // dt) — not a realistic scenario at real frame rates, just an artifact of
    // ticking by hand.
    for (int i = 0; i < 11; i++) actor->update(0.05f); // ~0.55s: walk completes, say has just begun

    REQUIRE(actor->getPosition().x == 15);
    REQUIRE(actor->getPosition().y == 25);
    REQUIRE(actor->getTextOver().toString() == "Hi there");

    // Let the say run out so the playback finishes cleanly (see
    // Cutscene::isPlaying()).
    for (int i = 0; i < 12; i++) actor->update(0.05f);

    std::remove(path);
}

TEST_CASE("CutsceneLoader resolves moveCamera against the current scene and changeScene by name", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();

    auto sceneA = std::make_shared<Scene>();
    auto sceneB = std::make_shared<Scene>();
    game.registerScene("loader_scene_a", sceneA);
    game.registerScene("loader_scene_b", sceneB);
    game.changeScene(sceneA, 0.0f);

    const char* path = "test_cutscene_camera_scene.json";
    writeFile(path, R"({
        "name": "loader_camera_scene",
        "steps": [
            { "type": "moveCamera", "position": [40, 40], "seconds": 0.3 },
            { "type": "changeScene", "scene": "loader_scene_b" }
        ]
    })");

    Event cs = CutsceneLoader::loadCutscene(path);
    cs.execute();
    sceneA->getCamera().update(0.4f);

    REQUIRE(sceneA->getCamera().getPosition().x == 40);
    REQUIRE(game.getScene() == sceneB);

    std::remove(path);
}

TEST_CASE("CutsceneLoader resolves disableMovementFor by player name", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();
    auto player = std::make_shared<Player>("loader_dmf_player", Vector2<>(0, 0), Vector2<>(10, 10));
    game.addPlayer(player);

    const char* path = "test_cutscene_dmf.json";
    writeFile(path, R"({
        "name": "loader_dmf",
        "disableMovementFor": "loader_dmf_player",
        "steps": [ { "type": "wait", "seconds": 0.2 } ]
    })");

    Event cs = CutsceneLoader::loadCutscene(path);
    cs.execute();
    REQUIRE_FALSE(player->canMove());

    Event::tick(0.3f);
    REQUIRE(player->canMove());

    std::remove(path);
}

TEST_CASE("CutsceneLoader's script step runs a registered script and can wait afterward", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();

    int ran = 0;
    game.registerScript("loader_cutscene_script", [&ran]() { ran++; });

    const char* path = "test_cutscene_script.json";
    writeFile(path, R"({
        "name": "loader_script_test",
        "steps": [
            { "type": "script", "name": "loader_cutscene_script", "waitSeconds": 0.3 },
            { "type": "script", "name": "loader_cutscene_script" }
        ]
    })");

    Event cs = CutsceneLoader::loadCutscene(path);
    cs.execute();
    REQUIRE(ran == 1); // first script ran; the wait blocks the second

    Event::tick(0.4f);
    REQUIRE(ran == 2);

    std::remove(path);
}

TEST_CASE("CutsceneLoader parses a parallel step into branches", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();
    auto scene = std::make_shared<Scene>();
    game.changeScene(scene, 0.0f);

    auto a = std::make_shared<GameObject>("loader_parallel_a", Vector2<>(0, 0), Vector2<>(10, 10));
    auto b = std::make_shared<GameObject>("loader_parallel_b", Vector2<>(0, 0), Vector2<>(10, 10));
    scene->addGameObject(a);
    scene->addGameObject(b);

    const char* path = "test_cutscene_parallel.json";
    writeFile(path, R"({
        "name": "loader_parallel_test",
        "steps": [
            { "type": "parallel", "branches": [
                [ { "type": "walk", "object": "loader_parallel_a", "position": [5, 5], "seconds": 0.2 } ],
                [ { "type": "walk", "object": "loader_parallel_b", "position": [9, 9], "seconds": 0.2 } ]
            ] }
        ]
    })");

    Event cs = CutsceneLoader::loadCutscene(path);
    cs.execute();
    a->update(0.3f);
    b->update(0.3f);

    REQUIRE(a->getPosition().x == 5);
    REQUIRE(b->getPosition().x == 9);

    std::remove(path);
}

TEST_CASE("CutsceneLoader::loadCutscene returns an empty event for a missing or malformed file", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    REQUIRE_NOTHROW(CutsceneLoader::loadCutscene("definitely_missing_cutscene.json").execute());

    const char* path = "test_cutscene_malformed.json";
    writeFile(path, "{ not valid json");
    REQUIRE_NOTHROW(CutsceneLoader::loadCutscene(path).execute());
    std::remove(path);
}

TEST_CASE("CutsceneLoader logs and skips an unknown step type but still runs the rest", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    std::vector<int> order;
    Game& game = Game::getInstance();
    game.registerScript("loader_unknown_step_marker", [&order]() { order.push_back(1); });

    const char* path = "test_cutscene_unknown_step.json";
    writeFile(path, R"({
        "name": "loader_unknown_step",
        "steps": [
            { "type": "notARealStepType" },
            { "type": "script", "name": "loader_unknown_step_marker" }
        ]
    })");

    Event cs = CutsceneLoader::loadCutscene(path);
    cs.execute();
    REQUIRE(order == std::vector<int>{1});

    std::remove(path);
}

TEST_CASE("CutsceneLoader::loadAndRegister registers a runnable script and fails for a missing file", "[cutscene][loader]") {
    drainPendingEvents();
    ensureCurrentPlayer();
    Game& game = Game::getInstance();

    int ran = 0;
    game.registerScript("loader_register_inner", [&ran]() { ran++; });

    const char* path = "test_cutscene_register.json";
    writeFile(path, R"({
        "name": "loader_register_test",
        "steps": [ { "type": "script", "name": "loader_register_inner" } ]
    })");

    REQUIRE(CutsceneLoader::loadAndRegister(game, path, "loader_registered_cutscene"));
    REQUIRE(game.hasScript("loader_registered_cutscene"));

    game.scriptEvent("loader_registered_cutscene").execute();
    REQUIRE(ran == 1);

    REQUIRE_FALSE(CutsceneLoader::loadAndRegister(game, "definitely_missing.json", "wont_exist"));

    std::remove(path);
}
