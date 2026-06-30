#include "ScriptLoader.hpp"
#include "EngineAPI.hpp"
#include "ResourceManager.hpp"
#include <SDL3/SDL.h>
#include <filesystem>

int main(int argc, char* argv[]) {
	const char* basePath = SDL_GetBasePath();
	std::filesystem::path assetsDir = std::filesystem::weakly_canonical(
		std::filesystem::path(basePath) / "../../assets");
	ResourceManager::init(assetsDir.string());
	SDL_free((void*)basePath);

	ScriptLoader loader;
	Game& game = Game::getInstance();
	loader.loadGame(game);
	game.startGame();
	return 0;
}