#pragma once

#include "Texture.hpp"
#include <SDL3/SDL.h>
#include <map>
#include <SDL3_ttf/SDL_ttf.h>

#define TEXTURES_DIRECTORY "../assets/textures/"
#define FONTS_DIRECTORY "../assets/fonts/"

class ResourceManager {
	std::map<std::string, SDL_Texture*> cachedSources;
	std::map<std::string, TTF_Font*> cachedFonts;
public:
	ResourceManager();
	//SDL_Surface* loadTexture(Texture texture);
	SDL_Texture* loadTexture(SDL_Renderer* sdlRenderer, Texture& texture, SDL_FRect* srcRect);
	TTF_Font* loadFont(std::string fontName, int fontSize);
	~ResourceManager();
};