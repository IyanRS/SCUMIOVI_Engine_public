#pragma once

#include "Texture.hpp"
#include <SDL3/SDL.h>
#include <map>
#include <SDL3_ttf/SDL_ttf.h>

#define TEXTURES_DIRECTORY "../assets/textures/"
#define FONTS_DIRECTORY "../assets/fonts/"

class ResourceManager {
	// Singleton
protected:
	ResourceManager();
	static ResourceManager* resourceManager_;

public:
	ResourceManager(ResourceManager& other) = delete;
	void operator=(const ResourceManager&) = delete;
	static ResourceManager& getInstance();

	std::map<std::string, SDL_Texture*> cachedTextures;
	std::map<std::string, SDL_Surface*> cachedSurfaces;
	std::map<std::string, TTF_Font*> cachedFonts;
public:
	//ResourceManager();
	//SDL_Surface* loadTexture(Texture texture);
	SDL_Texture* loadTexture(SDL_Renderer* sdlRenderer, Texture& texture, SDL_FRect* srcRect);
	SDL_Surface* loadSurface(Texture& texture);
	TTF_Font* loadFont(std::string fontName, int fontSize);
	Vector2<int> getImageSize(std::string filename);
	~ResourceManager();
};