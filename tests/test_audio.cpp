#include "catch.hpp"
#include "AudioManager.hpp"
#include "Game.hpp"

TEST_CASE("AudioManager constructor and destructor", "[audio]") {
    AudioManager* am = new AudioManager();
    REQUIRE(am != nullptr);
    delete am;
}

TEST_CASE("AudioManager master volume get/set", "[audio]") {
    AudioManager am;
    REQUIRE(am.getMasterVolume() == 1.0f);
    am.setMasterVolume(0.5f);
    REQUIRE(am.getMasterVolume() == 0.5f);
    am.setMasterVolume(0.0f);
    REQUIRE(am.getMasterVolume() == 0.0f);
    am.setMasterVolume(1.0f);
    REQUIRE(am.getMasterVolume() == 1.0f);
}

TEST_CASE("AudioManager isMusicPlaying false when idle", "[audio]") {
    AudioManager am;
    REQUIRE(am.isMusicPlaying() == false);
}

TEST_CASE("AudioManager loadSound with bad path returns false", "[audio]") {
    AudioManager am;
    REQUIRE(am.loadSound("test", "nonexistent.wav") == false);
}

TEST_CASE("AudioManager playMusic with bad path returns false", "[audio]") {
    AudioManager am;
    REQUIRE(am.playMusic("nonexistent.wav") == false);
}

TEST_CASE("AudioManager playSound with unknown name does not crash", "[audio]") {
    AudioManager am;
    REQUIRE_NOTHROW(am.playSound("does_not_exist"));
}

TEST_CASE("AudioManager stopMusic safe when nothing playing", "[audio]") {
    AudioManager am;
    REQUIRE_NOTHROW(am.stopMusic());
}

TEST_CASE("AudioManager pauseMusic safe when nothing playing", "[audio]") {
    AudioManager am;
    REQUIRE_NOTHROW(am.pauseMusic());
}

TEST_CASE("AudioManager resumeMusic safe when nothing playing", "[audio]") {
    AudioManager am;
    REQUIRE_NOTHROW(am.resumeMusic());
}

TEST_CASE("AudioManager update does not crash when empty", "[audio]") {
    AudioManager am;
    REQUIRE_NOTHROW(am.update());
}

TEST_CASE("AudioManager double stopMusic safe", "[audio]") {
    AudioManager am;
    am.stopMusic();
    REQUIRE_NOTHROW(am.stopMusic());
}

TEST_CASE("AudioManager init can be called (may fail without audio device)", "[audio]") {
    AudioManager am;
    bool result = am.init();
    REQUIRE_NOTHROW(am.update());
    REQUIRE_NOTHROW(am.playSound("whatever"));
    REQUIRE_NOTHROW(am.playMusic("whatever.wav"));
    if (result) {
        REQUIRE(am.isMusicPlaying() == false);
    }
}

TEST_CASE("AudioManager static init after SDL_Init is safe", "[audio]") {
    Game& game = Game::getInstance();
    AudioManager& audio = game.getAudio();
    REQUIRE_NOTHROW(audio.update());
    REQUIRE_NOTHROW(audio.playSound("nonexistent"));
    REQUIRE_NOTHROW(audio.stopMusic());
    REQUIRE_NOTHROW(audio.pauseMusic());
    REQUIRE_NOTHROW(audio.resumeMusic());
}

TEST_CASE("AudioManager masterVolume bounds", "[audio]") {
    AudioManager am;
    am.setMasterVolume(0.0f);
    REQUIRE(am.getMasterVolume() == 0.0f);
    am.setMasterVolume(1.0f);
    REQUIRE(am.getMasterVolume() == 1.0f);
    am.setMasterVolume(0.333f);
    REQUIRE(am.getMasterVolume() == 0.333f);
}

TEST_CASE("AudioManager multiple loads of same name replaces", "[audio]") {
    AudioManager am;
    REQUIRE(am.loadSound("coin", "missing1.wav") == false);
    REQUIRE(am.loadSound("coin", "missing2.wav") == false);
    REQUIRE_NOTHROW(am.playSound("coin"));
}
