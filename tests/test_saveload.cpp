#include "catch.hpp"
#include "Game.hpp"
#include "GameState.hpp"
#include "Scene.hpp"
#include "GameObject.hpp"
#include "Player.hpp"
#include "json.hpp"
#include <fstream>

static void drainPendingEvents() {
	for (int i = 0; i < 100; i++) Event::tick(10.0f);
}

TEST_CASE("GameState to_json / from_json round-trip", "[saveload][gamestate]") {
	drainPendingEvents();

	GameState gs;
	gs.setFlag("has_key");
	gs.setFlag("door_open", false);
	gs.setCounter("score", 42);
	gs.incrementCounter("items", 3);

	nlohmann::json j = gs.to_json();
	REQUIRE(j["flags"]["has_key"] == true);
	REQUIRE(j["flags"]["door_open"] == false);
	REQUIRE(j["counters"]["score"] == 42);
	REQUIRE(j["counters"]["items"] == 3);

	GameState loaded;
	loaded.from_json(j);
	REQUIRE(loaded.getFlag("has_key") == true);
	REQUIRE(loaded.getFlag("door_open") == false);
	REQUIRE(loaded.getCounter("score") == 42);
	REQUIRE(loaded.getCounter("items") == 3);
}

TEST_CASE("GameState from_json handles empty JSON", "[saveload][gamestate]") {
	drainPendingEvents();

	GameState gs;
	nlohmann::json j;
	gs.from_json(j);
	REQUIRE(gs.getFlag("anything") == false);
	REQUIRE(gs.getCounter("anything") == 0);
}

TEST_CASE("Scene name set/get", "[saveload][scene]") {
	drainPendingEvents();

	auto scene = std::make_shared<Scene>();
	REQUIRE(scene->getName().empty());

	scene->setName("scummbar");
	REQUIRE(scene->getName() == "scummbar");
}

TEST_CASE("Scene registry", "[saveload][game]") {
	drainPendingEvents();

	Game& game = Game::getInstance();
	game.clearSceneRegistry();

	auto scene = std::make_shared<Scene>();
	game.registerScene("test_scene", scene);
	REQUIRE(scene->getName() == "test_scene");

	auto found = game.findScene("test_scene");
	REQUIRE(found == scene);

	auto notFound = game.findScene("nonexistent");
	REQUIRE(notFound == nullptr);
}

TEST_CASE("GameObject setVisible / isVisible", "[saveload][gameobject]") {
	drainPendingEvents();

	GameObject obj("test", Vector2<>(0, 0), Vector2<>(10, 10));
	REQUIRE(obj.isVisible() == true);

	obj.setVisible(false);
	REQUIRE(obj.isVisible() == false);

	obj.setVisible(true);
	REQUIRE(obj.isVisible() == true);
}

TEST_CASE("Inventory prototype registry", "[saveload][game]") {
	drainPendingEvents();

	Game& game = Game::getInstance();
	auto proto = std::make_shared<GameObject>("mug", Vector2<>(), Vector2<>(-1, -1));

	game.registerInventoryItemPrototype("mug", proto);
	auto found = game.findInventoryItemPrototype("mug");
	REQUIRE(found == proto);

	auto notFound = game.findInventoryItemPrototype("nonexistent");
	REQUIRE(notFound == nullptr);
}

TEST_CASE("saveGame and loadGame round-trip", "[saveload][game]") {
	drainPendingEvents();

	Game& game = Game::getInstance();

	// Set up state
	game.getGameState().setFlag("test_flag");
	game.getGameState().setCounter("test_counter", 99);

	auto scene = std::make_shared<Scene>();
	scene->setName("test_scene");
	auto obj = std::make_shared<GameObject>("test_obj", Vector2<>(10, 20), Vector2<>(30, 40));
	obj->setState(5);
	obj->setVisible(false);
	scene->addGameObject(obj);

	game.registerScene("test_scene", scene);

	auto player = std::make_shared<Player>("test_player", Vector2<>(15, 25), Vector2<>(10, 10));
	game.addPlayer(player);
	game.setCurrentPlayer(player);
	player->moveToScene(scene);

	game.changeScene(scene, 0.0f);
	drainPendingEvents();

	// Save
	REQUIRE(game.saveGame(99) == true);

	// Verify file exists
	std::ifstream check("save_99.json");
	REQUIRE(check.good());

	// Read back the raw JSON to verify structure
	nlohmann::json saved;
	check >> saved;
	check.close();

	REQUIRE(saved["gameState"]["flags"]["test_flag"] == true);
	REQUIRE(saved["gameState"]["counters"]["test_counter"] == 99);
	REQUIRE(saved["currentScene"] == "test_scene");
	REQUIRE(saved["currentPlayerName"] == "test_player");
	REQUIRE(saved["players"]["test_player"]["position"]["x"] == 15);
	REQUIRE(saved["players"]["test_player"]["position"]["y"] == 25);
	REQUIRE(saved["players"]["test_player"]["scene"] == "test_scene");
	REQUIRE(saved["scenes"]["test_scene"]["test_obj"]["state"] == 5);
	REQUIRE(saved["scenes"]["test_scene"]["test_obj"]["visible"] == false);

	// Clean up
	std::remove("save_99.json");
}

TEST_CASE("loadGame fails gracefully on missing file", "[saveload][game]") {
	drainPendingEvents();
	Game& game = Game::getInstance();
	REQUIRE(game.loadGame(9999) == false);
}

TEST_CASE("applyObjectStates restores object state and visibility", "[saveload][game]") {
	drainPendingEvents();

	Game& game = Game::getInstance();

	auto scene = std::make_shared<Scene>();
	scene->setName("restore_scene");
	auto obj = std::make_shared<GameObject>("restore_obj", Vector2<>(0, 0), Vector2<>(10, 10));
	scene->addGameObject(obj);
	game.registerScene("restore_scene", scene);

	// Apply states
	nlohmann::json data;
	data["restore_scene"]["restore_obj"]["state"] = 7;
	data["restore_scene"]["restore_obj"]["visible"] = false;
	data["restore_scene"]["restore_obj"]["position"]["x"] = 100.0f;
	data["restore_scene"]["restore_obj"]["position"]["y"] = 200.0f;

	game.applyObjectStates(data);

	REQUIRE(obj->getState() == 7);
	REQUIRE(obj->isVisible() == false);
	REQUIRE(obj->getPosition().x == 100.0f);
	REQUIRE(obj->getPosition().y == 200.0f);
}

TEST_CASE("applyInventory restores items from prototypes", "[saveload][game]") {
	drainPendingEvents();

	Game& game = Game::getInstance();
	auto player = std::make_shared<Player>("inv_player", Vector2<>(0, 0), Vector2<>(10, 10));
	game.addPlayer(player);
	game.setCurrentPlayer(player);

	auto proto = std::make_shared<GameObject>("mug", Vector2<>(), Vector2<>(-1, -1));
	game.registerInventoryItemPrototype("mug", proto);

	REQUIRE(player->getInventory().getItems().empty());

	game.applyInventory(player, {"mug"});

	REQUIRE(player->getInventory().getItems().size() == 1);
	REQUIRE(player->getInventory().getItems()[0]->getName() == "mug");

	// Clear for other tests
	player->getInventory().getItems().clear();
}

TEST_CASE("quitGame stops the game loop", "[saveload][game]") {
	drainPendingEvents();
	Game& game = Game::getInstance();
	game.quitGame();
}

TEST_CASE("saveGame persists every party member, not just the current one", "[saveload][game][party]") {
	drainPendingEvents();
	Game& game = Game::getInstance();

	auto sceneA = std::make_shared<Scene>();
	sceneA->setName("party_save_scene_a");
	auto sceneB = std::make_shared<Scene>();
	sceneB->setName("party_save_scene_b");
	game.registerScene("party_save_scene_a", sceneA);
	game.registerScene("party_save_scene_b", sceneB);

	auto a = std::make_shared<Player>("party_save_a", Vector2<>(1, 2), Vector2<>(10, 10));
	auto b = std::make_shared<Player>("party_save_b", Vector2<>(3, 4), Vector2<>(10, 10));
	game.addPlayer(a);
	game.addPlayer(b);
	a->moveToScene(sceneA);
	b->moveToScene(sceneB);
	game.setCurrentPlayer(a);
	game.changeScene(sceneA, 0.0f);
	drainPendingEvents();

	REQUIRE(game.saveGame(98) == true);

	std::ifstream check("save_98.json");
	nlohmann::json saved;
	check >> saved;
	check.close();

	REQUIRE(saved["currentPlayerName"] == "party_save_a");
	REQUIRE(saved["players"]["party_save_a"]["scene"] == "party_save_scene_a");
	REQUIRE(saved["players"]["party_save_b"]["scene"] == "party_save_scene_b");
	REQUIRE(saved["players"]["party_save_b"]["position"]["x"] == 3);
	REQUIRE(saved["players"]["party_save_b"]["position"]["y"] == 4);

	std::remove("save_98.json");
}

TEST_CASE("loadGame restores every party member's scene/position/inventory", "[saveload][game][party]") {
	drainPendingEvents();
	Game& game = Game::getInstance();

	auto sceneA = std::make_shared<Scene>();
	sceneA->setName("party_load_scene_a");
	auto sceneB = std::make_shared<Scene>();
	sceneB->setName("party_load_scene_b");
	game.registerScene("party_load_scene_a", sceneA);
	game.registerScene("party_load_scene_b", sceneB);

	auto a = std::make_shared<Player>("party_load_a", Vector2<>(0, 0), Vector2<>(10, 10));
	auto b = std::make_shared<Player>("party_load_b", Vector2<>(0, 0), Vector2<>(10, 10));
	game.addPlayer(a);
	game.addPlayer(b);
	a->moveToScene(sceneA);
	b->moveToScene(sceneB);

	auto proto = std::make_shared<GameObject>("party_load_item", Vector2<>(), Vector2<>(-1, -1));
	game.registerInventoryItemPrototype("party_load_item", proto);
	b->getInventory().addItem(std::make_shared<GameObject>("party_load_item", Vector2<>(), Vector2<>(-1, -1)));

	game.setCurrentPlayer(a);
	game.changeScene(sceneA, 0.0f);
	drainPendingEvents();

	REQUIRE(game.saveGame(97) == true);

	// Mutate everything after saving
	a->setPosition(Vector2<>(999, 999));
	b->setPosition(Vector2<>(888, 888));
	b->getInventory().getItems().clear();
	game.setCurrentPlayer(b);

	REQUIRE(game.loadGame(97) == true);

	REQUIRE(game.getCurrentPlayer() == a);
	REQUIRE(a->getPosition().x == 0);
	REQUIRE(b->getPosition().x == 0);
	REQUIRE(b->getScene() == sceneB);
	REQUIRE(b->getInventory().getItems().size() == 1);
	REQUIRE(b->getInventory().getItems()[0]->getName() == "party_load_item");

	std::remove("save_97.json");
}

TEST_CASE("loadGame cancels in-progress movement so the player doesn't finish an old walk", "[saveload][game]") {
	drainPendingEvents();
	Game& game = Game::getInstance();

	auto scene = std::make_shared<Scene>();
	scene->setName("move_cancel_scene");
	game.registerScene("move_cancel_scene", scene);

	auto player = std::make_shared<Player>("move_cancel_player", Vector2<>(0, 0), Vector2<>(10, 10));
	game.addPlayer(player);
	player->moveToScene(scene);
	game.setCurrentPlayer(player);
	game.changeScene(scene, 0.0f);
	drainPendingEvents();

	REQUIRE(game.saveGame(96) == true); // saves position (0,0)

	// Start walking toward a far point, then immediately load an older save —
	// the player should end up exactly at the loaded position and NOT keep
	// interpolating toward (500,500) on subsequent frames.
	player->moveTo(Vector2<>(500, 500), 10.0f);
	REQUIRE(game.loadGame(96) == true);

	REQUIRE(player->getPosition().x == 0);
	REQUIRE(player->getPosition().y == 0);

	scene->update(1.0f); // would move the player further along the old path if not cancelled
	REQUIRE(player->getPosition().x == 0);
	REQUIRE(player->getPosition().y == 0);

	std::remove("save_96.json");
}
