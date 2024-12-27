#include "ScriptLoader.hpp"

void test() {
	SDL_Log("Test");
}

int main(int argc, char* argv[]) {
	ScriptLoader scriptLoader = ScriptLoader();
	scriptLoader.loadAndRunGame();

	return 0;
}