#include "catch.hpp"
#include "Localization.hpp"
#include <fstream>
#include <cstdio>

TEST_CASE("tr returns the source string in the source locale", "[localization]") {
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.setLocale("en");
    REQUIRE(tr("Walk to") == "Walk to");
    REQUIRE(tr("anything untranslated") == "anything untranslated");
}

TEST_CASE("tr returns the translation for the active locale", "[localization]") {
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.addTranslation("es", "Walk to", "Caminar a");
    loc.setLocale("es");
    REQUIRE(tr("Walk to") == "Caminar a");
    loc.setLocale("en");
}

TEST_CASE("tr falls back to the source when a key is missing in the locale", "[localization]") {
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.addTranslation("es", "Hi", "Hola");
    loc.setLocale("es");
    REQUIRE(tr("Hi") == "Hola");
    REQUIRE(tr("Bye") == "Bye");
    loc.setLocale("en");
}

TEST_CASE("switching locale at runtime changes resolution", "[localization]") {
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.addTranslation("es", "Give", "Dar");
    REQUIRE(tr("Give") == "Give"); // still English
    loc.setLocale("es");
    REQUIRE(tr("Give") == "Dar");
    loc.setLocale("en");
    REQUIRE(tr("Give") == "Give");
}

TEST_CASE("placeholders are preserved through translation", "[localization]") {
    auto& loc = Localization::getInstance();
    loc.clear();
    loc.addTranslation("es", "Use $selected on $target", "Usar $selected con $target");
    loc.setLocale("es");
    REQUIRE(tr("Use $selected on $target") == "Usar $selected con $target");
    loc.setLocale("en");
}

TEST_CASE("loadLocaleFile merges source:translation pairs", "[localization]") {
    auto& loc = Localization::getInstance();
    loc.clear();
    const char* path = "test_locale_tmp.json";
    {
        std::ofstream f(path);
        f << "{ \"Open\": \"Abrir\", \"Close\": \"Cerrar\" }";
    }
    REQUIRE(loc.loadLocaleFile("es", path) == true);
    loc.setLocale("es");
    REQUIRE(tr("Open") == "Abrir");
    REQUIRE(tr("Close") == "Cerrar");
    loc.setLocale("en");
    std::remove(path);
}

TEST_CASE("loadLocaleFile returns false for a missing file", "[localization]") {
    auto& loc = Localization::getInstance();
    REQUIRE(loc.loadLocaleFile("es", "definitely_missing_locale_file.json") == false);
}
