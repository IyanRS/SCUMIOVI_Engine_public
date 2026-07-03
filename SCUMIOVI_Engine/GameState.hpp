#pragma once

#include <string>
#include <unordered_map>

class GameState {
	std::unordered_map<std::string, bool> flags;
	std::unordered_map<std::string, int> counters;
public:
	void setFlag(const std::string& key, bool value = true);
	bool getFlag(const std::string& key) const;
	void clearFlag(const std::string& key);

	void setCounter(const std::string& key, int value);
	int getCounter(const std::string& key) const;
	void incrementCounter(const std::string& key, int delta = 1);
};
