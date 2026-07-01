#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <unordered_map>

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	bool init();
	void update();

	bool loadSound(const std::string& name, const std::string& filepath);
	void playSound(const std::string& name, float volume = 1.0f);

	bool playMusic(const std::string& filepath, bool loop = true);
	void stopMusic();
	void pauseMusic();
	void resumeMusic();
	bool isMusicPlaying() const;

	void setMasterVolume(float volume);
	float getMasterVolume() const;

private:
	struct SoundData {
		Uint8* data = nullptr;
		Uint32 len = 0;
		SDL_AudioSpec spec;
	};

	struct MusicTrack {
		Uint8* data = nullptr;
		Uint32 len = 0;
		SDL_AudioSpec spec;
		bool looping = false;
	};

	static const int MAX_SFX_STREAMS = 8;

	SDL_AudioDeviceID device = 0;
	SDL_AudioStream* musicStream = nullptr;
	SDL_AudioStream* sfxStreams[MAX_SFX_STREAMS] = {};
	std::unordered_map<std::string, SoundData> soundCache;
	MusicTrack currentMusic;
	float masterVolume = 1.0f;

	int findFreeSFXStream();
};
