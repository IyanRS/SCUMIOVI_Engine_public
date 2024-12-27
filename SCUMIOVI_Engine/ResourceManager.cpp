#include "ResourceManager.hpp"
#include <SDL3_image/SDL_image.h>

//SDL_Surface* ResourceManager::loadTexture(Texture texture) {
SDL_Texture* ResourceManager::loadTexture(SDL_Renderer* sdlRenderer, Texture& texture, SDL_FRect* srcRect) {
	std::string textureFilePath = TEXTURES_DIRECTORY + texture.getSource();
	//SDL_Surface* surface = IMG_Load(textureFilePath.c_str());
	//if (surface == nullptr) {
	//	SDL_Log("Could not load surface: %s.", SDL_GetError());
	//	// TODO: Return some invalid texture placeholder
	//}
	//SDL_Texture* textureImg = SDL_CreateTextureFromSurface(sdlRenderer, surface);
	//if (textureImg == nullptr) {
	//	SDL_Log("Could not load texture: %s.", SDL_GetError());
	//	// TODO: Return some invalid texture placeholder
	//}

	SDL_Texture* textureImg;

	std::map<std::string, SDL_Texture*>::iterator it = cachedSources.find(texture.getSource());
	if (it != cachedSources.end()) {
		textureImg = it->second;
	}
	else {
		textureImg = IMG_LoadTexture(sdlRenderer, textureFilePath.c_str());
		if (textureImg == nullptr) {
			SDL_Log("Could not load texture, a placeholder will be used: %s.", SDL_GetError());
			// TODO: Return some invalid texture placeholder
		}
		cachedSources[texture.getSource()] = textureImg;
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
	//if (texture.getSize().x == -1) { // -1 Means whole image width
	//	srcRect->w = textureImg->w;
	//}
	//else {
	//	srcRect->w = texture.getSize().x;
	//}
	//if (texture.getSize().y == -1) { // -1 Means whole image height
	//	srcRect->h = textureImg->h;
	//}
	//else {
	//	srcRect->h = texture.getSize().y;
	//}
	srcRect->w = texture.getSize().x;
	srcRect->h = texture.getSize().y;

	//SDL_SetTextureScaleMode(textureImg, SDL_SCALEMODE_NEAREST);

	return textureImg; //TODO: store and free memory on object destroy
}

ResourceManager::~ResourceManager() {
	std::map<std::string, SDL_Texture*>::iterator it;

	for (it = cachedSources.begin(); it != cachedSources.end(); it++)
	{
		SDL_DestroyTexture(it->second);
	}
}