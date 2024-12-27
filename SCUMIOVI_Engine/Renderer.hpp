#pragma once

#include "Scene.hpp"
#include <SDL3/SDL.h>
//#include "UI.hpp"
#include "ResourceManager.hpp"

#include <chrono>
#include <vector>
#include <array>

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
public:
	Renderer();
	~Renderer();
	void render(Scene scene);
	void renderDebugElements(Scene scene);
	//void render(UI ui);
};