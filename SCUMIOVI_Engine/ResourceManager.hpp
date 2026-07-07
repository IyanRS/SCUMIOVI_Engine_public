#pragma once

#include "Texture.hpp"
#include <SDL3/SDL.h>
#include <map>
#include <string>
#include <SDL3_ttf/SDL_ttf.h>

class ResourceManager {
protected:
	ResourceManager();
	static ResourceManager* resourceManager_;

public:
	ResourceManager(ResourceManager& other) = delete;
	void operator=(const ResourceManager&) = delete;
	static ResourceManager& getInstance();

	static void init(const std::string& assetsDirectory);
	static const std::string& getTexturesDirectory();
	static const std::string& getFontsDirectory();
	static const std::string& getLocalesDirectory();
	static const std::string& getUIDirectory();
	static const std::string& getKeybindingsPath();

	std::map<std::string, SDL_Texture*> cachedTextures;
	std::map<std::string, SDL_Surface*> cachedSurfaces;
	std::map<std::string, TTF_Font*> cachedFonts;

	SDL_Texture* loadTexture(SDL_Renderer* sdlRenderer, Texture& texture, SDL_FRect* srcRect);
	SDL_Surface* loadSurface(Texture& texture);
	TTF_Font* loadFont(std::string fontName, int fontSize);
	Vector2<int> getImageSize(std::string filename);
	~ResourceManager();

private:
	static std::string texturesDirectory_;
	static std::string fontsDirectory_;
	static std::string localesDirectory_;
	static std::string uiDirectory_;
	static std::string keybindingsPath_;
};