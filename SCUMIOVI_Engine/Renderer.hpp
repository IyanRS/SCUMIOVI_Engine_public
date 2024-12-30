#pragma once

#include "Scene.hpp"
#include <SDL3/SDL.h>
//#include "UI.hpp"
#include "ResourceManager.hpp"

#include <chrono>
#include <vector>
#include <array>

#include "UI.hpp"

class Renderer {
	SDL_Window* sdlWindow;
	SDL_Renderer* sdlRenderer;
	ResourceManager resourceManager;
	void applyCameraTransform(Camera& camera, SDL_FRect& rect);
	void applyScreenTransform(SDL_FRect& rect);
	void applyCameraTransform(Camera& camera, Vector2<>& point);
	void applyScreenTransform(Vector2<>& point);
	std::chrono::system_clock::time_point lastRenderTime;
	std::vector<std::array<GameObject*, 2>> currentlyOverlapping;
	void renderGameObject(GameObject& gameObject, Camera& camera);
	void checkOverlaps(GameObject& gameObject1, GameObject& gameObject2);
	void renderText(Vector2<>, std::string, TextOverMode);
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