#pragma once

#include "Texture.hpp"
#include <SDL3/SDL.h>
#include <map>

#define TEXTURES_DIRECTORY "../assets/textures/"

class ResourceManager {
	std::map<std::string, SDL_Texture*> cachedSources;
public:
	//SDL_Surface* loadTexture(Texture texture);
	SDL_Texture* loadTexture(SDL_Renderer* sdlRenderer, Texture& texture, SDL_FRect* srcRect);
	~ResourceManager();
};