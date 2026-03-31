#pragma once

#include "Scene.hpp"
#include <SDL3/SDL.h>
//#include "UI.hpp"
#include "ResourceManager.hpp"

#include <vector>
#include <array>

#include "UI.hpp"

#include "Common.hpp"

class Renderer {
	Camera zeroCamera; // Dummy camera at 0, 0
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	ResourceManager& resourceManager;
	void applyCameraTransform(Camera& camera, SDL_FRect& rect);
	void applyScreenTransform(SDL_FRect& rect, Vector2<> renderFrameSize = { RENDER_HEIGHT, RENDER_WIDTH });
	void applyCameraTransform(Camera& camera, Vector2<>& point);
	void applyScreenTransform(Vector2<>& point, Vector2<> renderFrameSize = { RENDER_HEIGHT, RENDER_WIDTH });
	std::vector<std::array<GameObject*, 2>> currentlyOverlapping;
	void renderGameObject(GameObject& gameObject, Camera& camera);
	void checkOverlaps(GameObject& gameObject1, GameObject& gameObject2);
	void renderText(Vector2<>, Text&, Camera& camera, bool hasOutline=false);
	std::string generateTextureKey(TTF_Font* font, const std::string& textString, const int color[3], bool hasOutline); // Used for hashing fonts
	void renderTTFText(Vector2<> pos, Vector2<> size, TTF_Font* font, const std::string& textString, const int color[3], bool hasOutline=false);
	void renderUI(UI& ui);
	void renderUIElement(GameObject& uiElement);
	void renderDebugElements(Scene& scene);
	void renderTexture(Vector2<> position, Vector2<> size, Texture* texture, Camera& camera);
	TTF_TextEngine* textEngine;
	SDL_Cursor* customCursor;

	Texture* mouseTexture;
	Vector2<> mouseHotspot;
public:
	Renderer();
	~Renderer();
	void render(Scene& scene, UI& ui);
	void setCustomMouse(Texture& mouseTexture, Vector2<> mouseHotspot);
	//void render(UI ui);
};