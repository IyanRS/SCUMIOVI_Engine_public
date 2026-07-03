#include "GameState.hpp"

void GameState::setFlag(const std::string& key, bool value) {
	flags[key] = value;
}

bool GameState::getFlag(const std::string& key) const {
	auto it = flags.find(key);
	if (it != flags.end())
		return it->second;
	return false;
}

void GameState::clearFlag(const std::string& key) {
	flags.erase(key);
}

void GameState::setCounter(const std::string& key, int value) {
	counters[key] = value;
}

int GameState::getCounter(const std::string& key) const {
	auto it = counters.find(key);
	if (it != counters.end())
		return it->second;
	return 0;
}

void GameState::incrementCounter(const std::string& key, int delta) {
	counters[key] = getCounter(key) + delta;
}
