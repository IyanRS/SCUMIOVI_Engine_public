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
	REQUIRE(saved["player"]["position"]["x"] == 15);
	REQUIRE(saved["player"]["position"]["y"] == 25);
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

	game.applyInventory({"mug"});

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
