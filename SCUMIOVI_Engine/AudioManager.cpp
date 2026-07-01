#include "AudioManager.hpp"

#include <SDL3/SDL.h>

AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {
	stopMusic();

	for (int i = 0; i < MAX_SFX_STREAMS; i++) {
		if (sfxStreams[i]) {
			SDL_ClearAudioStream(sfxStreams[i]);
			SDL_DestroyAudioStream(sfxStreams[i]);
		}
	}

	if (musicStream) {
		SDL_DestroyAudioStream(musicStream);
	}

	if (device != 0) {
		SDL_CloseAudioDevice(device);
	}

	for (auto& pair : soundCache) {
		SDL_free(pair.second.data);
	}
	soundCache.clear();
}

bool AudioManager::init() {
	if (device != 0) return true;

	if (!SDL_Init(SDL_INIT_AUDIO)) {
		SDL_Log("AudioManager: SDL_Init(SDL_INIT_AUDIO) failed: %s", SDL_GetError());
		return false;
	}

	device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
	if (device == 0) {
		SDL_Log("AudioManager: SDL_OpenAudioDevice failed: %s", SDL_GetError());
		return false;
	}

	musicStream = SDL_CreateAudioStream(NULL, NULL);
	if (!musicStream) {
		SDL_Log("AudioManager: SDL_CreateAudioStream failed: %s", SDL_GetError());
		SDL_CloseAudioDevice(device);
		device = 0;
		return false;
	}
	SDL_BindAudioStream(device, musicStream);

	return true;
}

void AudioManager::update() {
	if (currentMusic.data && currentMusic.looping && musicStream) {
		if ((Uint32)SDL_GetAudioStreamQueued(musicStream) < currentMusic.len / 2) {
			SDL_PutAudioStreamData(musicStream, currentMusic.data, currentMusic.len);
		}
	}
}

bool AudioManager::loadSound(const std::string& name, const std::string& filepath) {
	SoundData sound;
	if (!SDL_LoadWAV(filepath.c_str(), &sound.spec, &sound.data, &sound.len)) {
		SDL_Log("AudioManager: failed to load sound '%s' from '%s': %s", name.c_str(), filepath.c_str(), SDL_GetError());
		return false;
	}
	soundCache[name] = sound;
	return true;
}

void AudioManager::playSound(const std::string& name, float volume) {
	if (device == 0) return;

	auto it = soundCache.find(name);
	if (it == soundCache.end()) {
		SDL_Log("AudioManager: sound '%s' not found", name.c_str());
		return;
	}

	int idx = findFreeSFXStream();
	if (idx == -1) return;

	auto& sound = it->second;

	if (!sfxStreams[idx]) {
		sfxStreams[idx] = SDL_CreateAudioStream(&sound.spec, NULL);
		if (!sfxStreams[idx]) {
			SDL_Log("AudioManager: SDL_CreateAudioStream failed: %s", SDL_GetError());
			return;
		}
		SDL_BindAudioStream(device, sfxStreams[idx]);
	}

	SDL_SetAudioStreamFormat(sfxStreams[idx], &sound.spec, NULL);
	SDL_SetAudioStreamGain(sfxStreams[idx], volume * masterVolume);
	SDL_PutAudioStreamData(sfxStreams[idx], sound.data, sound.len);
}

bool AudioManager::playMusic(const std::string& filepath, bool loop) {
	if (device == 0) return false;

	stopMusic();

	SDL_AudioSpec spec;
	Uint8* buf;
	Uint32 len;
	if (!SDL_LoadWAV(filepath.c_str(), &spec, &buf, &len)) {
		SDL_Log("AudioManager: failed to load music from '%s': %s", filepath.c_str(), SDL_GetError());
		return false;
	}

	currentMusic.data = buf;
	currentMusic.len = len;
	currentMusic.spec = spec;
	currentMusic.looping = loop;

	SDL_SetAudioStreamFormat(musicStream, &spec, NULL);
	SDL_SetAudioStreamGain(musicStream, masterVolume);
	SDL_PutAudioStreamData(musicStream, buf, len);

	return true;
}

void AudioManager::stopMusic() {
	if (musicStream) {
		SDL_ClearAudioStream(musicStream);
	}
	if (currentMusic.data) {
		SDL_free(currentMusic.data);
		currentMusic.data = nullptr;
	}
	currentMusic.len = 0;
	currentMusic.looping = false;
}

void AudioManager::pauseMusic() {
	if (musicStream) {
		SDL_SetAudioStreamGain(musicStream, 0.0f);
	}
}

void AudioManager::resumeMusic() {
	if (musicStream) {
		SDL_SetAudioStreamGain(musicStream, masterVolume);
	}
}

bool AudioManager::isMusicPlaying() const {
	return currentMusic.data != nullptr;
}

void AudioManager::setMasterVolume(float volume) {
	masterVolume = volume;
	if (musicStream) {
		SDL_SetAudioStreamGain(musicStream, masterVolume);
	}
}

float AudioManager::getMasterVolume() const {
	return masterVolume;
}

int AudioManager::findFreeSFXStream() {
	for (int i = 0; i < MAX_SFX_STREAMS; i++) {
		if (sfxStreams[i] && SDL_GetAudioStreamQueued(sfxStreams[i]) == 0) {
			return i;
		}
	}
	return -1;
}
