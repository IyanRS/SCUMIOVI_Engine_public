#pragma once

class Game;

class GameDataLoader {
public:
	virtual ~GameDataLoader() = default;
	virtual void loadGame(Game& game) = 0;
};