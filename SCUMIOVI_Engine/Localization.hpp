#pragma once

#include <string>
#include <unordered_map>

// -----------------------------------------------------------------------------
// Localization
//
// Simple gettext-style string catalogue. User-facing strings are written in the
// source language (English) directly in engine code and generated game data, and
// that source string doubles as the lookup key. Translations for other locales
// are loaded from JSON files ({ "source string": "translation", ... }).
//
// Resolving a string in the source locale (or one with no matching entry) simply
// returns the source string, so English needs no data file and untranslated
// strings degrade gracefully instead of showing blank text or raw keys.
// -----------------------------------------------------------------------------
class Localization {
	std::string currentLocale = "en";
	std::string sourceLocale = "en";
	// locale -> (source string -> translation)
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> tables;

	Localization() = default;

public:
	Localization(const Localization&) = delete;
	void operator=(const Localization&) = delete;
	static Localization& getInstance();

	void setLocale(const std::string& locale);
	const std::string& getLocale() const;

	// The language the source strings are written in (default "en"). Strings are
	// returned verbatim when the current locale matches it.
	void setSourceLocale(const std::string& locale);

	// Register or replace a single translation.
	void addTranslation(const std::string& locale, const std::string& source, const std::string& translation);

	// Merge a locale table from a JSON file of { "source": "translation" } pairs.
	// Returns false if the file is missing or malformed.
	bool loadLocaleFile(const std::string& locale, const std::string& path);

	// Removes every loaded translation (used mainly by tests).
	void clear();

	// Resolve a source string to the current locale, falling back to the source.
	std::string translate(const std::string& source) const;
};

// Shorthand used throughout the engine and generated game code.
std::string tr(const std::string& source);
