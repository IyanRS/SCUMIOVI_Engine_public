#include "Localization.hpp"

#include <SDL3/SDL.h>
#include <fstream>
#include "json.hpp"

Localization& Localization::getInstance() {
	static Localization instance;
	return instance;
}

void Localization::setLocale(const std::string& locale) {
	currentLocale = locale;
}

const std::string& Localization::getLocale() const {
	return currentLocale;
}

void Localization::setSourceLocale(const std::string& locale) {
	sourceLocale = locale;
}

void Localization::addTranslation(const std::string& locale, const std::string& source, const std::string& translation) {
	tables[locale][source] = translation;
}

bool Localization::loadLocaleFile(const std::string& locale, const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		SDL_Log("Localization: could not open locale file '%s'", path.c_str());
		return false;
	}

	nlohmann::json data;
	try {
		file >> data;
	}
	catch (const std::exception& ex) {
		SDL_Log("Localization: failed to parse '%s': %s", path.c_str(), ex.what());
		return false;
	}

	if (!data.is_object()) {
		SDL_Log("Localization: '%s' must be a JSON object of source:translation pairs", path.c_str());
		return false;
	}

	auto& table = tables[locale];
	for (auto& [source, translation] : data.items()) {
		if (translation.is_string())
			table[source] = translation.get<std::string>();
	}
	return true;
}

void Localization::clear() {
	tables.clear();
}

std::string Localization::translate(const std::string& source) const {
	if (currentLocale == sourceLocale)
		return source;

	auto localeIt = tables.find(currentLocale);
	if (localeIt != tables.end()) {
		auto entry = localeIt->second.find(source);
		if (entry != localeIt->second.end())
			return entry->second;
	}
	return source;
}

std::string tr(const std::string& source) {
	return Localization::getInstance().translate(source);
}
