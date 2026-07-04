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

nlohmann::json GameState::to_json() const {
	nlohmann::json j;
	j["flags"] = flags;
	j["counters"] = counters;
	return j;
}

void GameState::from_json(const nlohmann::json& j) {
	if (j.contains("flags"))
		flags = j["flags"].get<std::unordered_map<std::string, bool>>();
	if (j.contains("counters"))
		counters = j["counters"].get<std::unordered_map<std::string, int>>();
}
