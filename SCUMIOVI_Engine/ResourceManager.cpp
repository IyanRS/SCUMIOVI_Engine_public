#include "ResourceManager.hpp"
#include <SDL3_image/SDL_image.h>

#include <SDL3/SDL.h>

ResourceManager::ResourceManager() {
	TTF_Init();
}

SDL_Surface* ResourceManager::loadSurface(Texture& texture) {
	if (texture.getSource() == "") {
		return nullptr;
	}

	std::string textureFilePath = TEXTURES_DIRECTORY + texture.getSource();

	SDL_Surface* surfaceImg;

	std::map<std::string, SDL_Surface*>::iterator it = cachedSurfaces.find(texture.getSource());
	if (it != cachedSurfaces.end()) {
		surfaceImg = it->second;
	}
	else {
		surfaceImg = IMG_Load(textureFilePath.c_str());
		if (surfaceImg == nullptr) {
			SDL_Log("Could not load texture: %s.", SDL_GetError());
			// TODO: Return some invalid texture placeholder
		}
		cachedSurfaces[texture.getSource()] = surfaceImg;
		int textureSizeX = texture.getSize().x;
		int textureSizeY = texture.getSize().y;
		if (textureSizeX == -1) {
			textureSizeX = surfaceImg->w;
		}
		if (textureSizeY == -1) {
			textureSizeY = surfaceImg->h;
		}
		texture.setSize(Vector2<int>(textureSizeX, textureSizeY));
	}

	return surfaceImg; //TODO: store and free memory on object destroy
}

//SDL_Surface* ResourceManager::loadTexture(Texture texture) {
SDL_Texture* ResourceManager::loadTexture(SDL_Renderer* sdlRenderer, Texture& texture, SDL_FRect* srcRect) {
	if (texture.getSource() == "") {
		return nullptr;
	}

	std::string textureFilePath = TEXTURES_DIRECTORY + texture.getSource();

	SDL_Texture* textureImg;

	std::map<std::string, SDL_Texture*>::iterator it = cachedTextures.find(texture.getSource());
	if (it != cachedTextures.end()) {
		textureImg = it->second;
	}
	else {
		textureImg = IMG_LoadTexture(sdlRenderer, textureFilePath.c_str());
		if (textureImg == nullptr) {
			SDL_Log("Could not load texture, a placeholder will be used: %s.", SDL_GetError());
			return nullptr;
			// TODO: Return some invalid texture placeholder instead?
		}
		cachedTextures[texture.getSource()] = textureImg;
		int textureSizeX = texture.getSize().x;
		int textureSizeY = texture.getSize().y;
		if (textureSizeX == -1) {
			textureSizeX = textureImg->w;
		}
		if (textureSizeY == -1) {
			textureSizeY = textureImg->h;
		}
		texture.setSize(Vector2<int>(textureSizeX, textureSizeY));

		SDL_SetTextureScaleMode(textureImg, SDL_SCALEMODE_NEAREST);
	}

	srcRect->x = texture.getPosition().x;
	srcRect->y = texture.getPosition().y;
	srcRect->w = texture.getSize().x;
	srcRect->h = texture.getSize().y;

	return textureImg;
}

ResourceManager::~ResourceManager() {
	std::map<std::string, SDL_Texture*>::iterator it;
	for (it = cachedTextures.begin(); it != cachedTextures.end(); it++)
	{
		SDL_DestroyTexture(it->second);
	}

	std::map<std::string, TTF_Font*>::iterator it1;
	for (it1 = cachedFonts.begin(); it1 != cachedFonts.end(); it1++)
	{
		TTF_CloseFont(it1->second);
	}

	std::map<std::string, SDL_Surface*>::iterator it2;
	for (it2 = cachedSurfaces.begin(); it2 != cachedSurfaces.end(); it2++)
	{
		SDL_DestroySurface(it2->second);
	}
}

TTF_Font* ResourceManager::loadFont(std::string fontFile, int size) {

	TTF_Font* font;

	std::map<std::string, TTF_Font*>::iterator it = cachedFonts.find(fontFile + std::to_string(size));
	if (it != cachedFonts.end()) {
		font = it->second;
	}
	else {
		font = TTF_OpenFont((FONTS_DIRECTORY + fontFile).c_str(), size);
		if (font == nullptr) {
			SDL_Log("TTF_OpenFont() Error: %s", SDL_GetError());
			return nullptr;
		}
		cachedFonts[fontFile + std::to_string(size)] = font;
	}
	
	return font;
}