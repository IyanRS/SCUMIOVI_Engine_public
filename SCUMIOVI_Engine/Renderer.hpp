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
	void renderGameObject(GameObject& gameObject, Camera& camera);
	void renderText(Vector2<>, Text&, Camera& camera, bool hasOutline=false);
	std::string generateTextureKey(TTF_Font* font, const std::string& textString, const int color[3], bool hasOutline); // Used for hashing fonts
	void renderTTFText(Vector2<> pos, Vector2<> size, TTF_Font* font, const std::string& textString, const int color[3], bool hasOutline=false);
	void renderUI(UI& ui);
	void renderUIElement(GameObject& uiElement);
	void renderDebugElements(Scene& scene);
	void renderTexture(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> texture, Camera& camera);
	TTF_TextEngine* textEngine;
	SDL_Cursor* customCursor;

	std::shared_ptr<Texture> mouseTexture;
	Vector2<> mouseHotspot;
	bool validState = true;

	float fadeAlpha = 0.0f;
	float fadeTarget = 0.0f;
	float fadeSpeed = 0.0f;
	Event fadeCompleteEvent;
	bool fading = false;
	void renderFadeOverlay();

public:
	Renderer();
	~Renderer();
	void render(Scene& scene, UI& ui);
	void setCustomMouse(std::shared_ptr<Texture> mouseTexture, Vector2<> mouseHotspot);
	void fadeToBlack(float duration, Event callback = Event());
	void fadeFromBlack(float duration, Event callback = Event());
	bool isFading() const;
	void updateFade(float deltaTime);
};