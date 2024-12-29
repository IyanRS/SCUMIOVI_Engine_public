#include "Renderer.hpp"

#include "Common.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <thread>

Renderer::Renderer() {
	int result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	if (result < 0) {
		SDL_Log("SDL_Init error %s", SDL_GetError());
	}

	sdlWindow = SDL_CreateWindow("SCUMIOVI", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (sdlWindow == NULL) {
		SDL_Log("SDL_CreateWindow error %s", SDL_GetError());
	}

	sdlRenderer = SDL_CreateRenderer(sdlWindow, NULL);
	if (sdlRenderer == NULL) {
		SDL_Log("SDL_CreateRenderer error %s", SDL_GetError());
	}

	textEngine = TTF_CreateRendererTextEngine(sdlRenderer);
	if (textEngine == NULL) {
		SDL_Log("TTF_CreateRendererTextEngine error %s", SDL_GetError());
	}

	lastRenderTime = std::chrono::system_clock::now();
}

Renderer::~Renderer() {
	SDL_DestroyRenderer(sdlRenderer);
	SDL_DestroyWindow(sdlWindow);
	TTF_DestroyRendererTextEngine(textEngine);
}

void Renderer::applyCameraTransform(Camera& camera, SDL_FRect& rect) {
	rect.x -= camera.getPosition().x;
	rect.y -= camera.getPosition().y;
}

void Renderer::applyScreenTransform(SDL_FRect& rect) {
	rect.x *= WINDOW_WIDTH / RENDER_WIDTH;
	rect.y *= WINDOW_HEIGHT / RENDER_HEIGHT;
	rect.w *= WINDOW_WIDTH / RENDER_WIDTH;
	rect.h *= WINDOW_HEIGHT / RENDER_HEIGHT;
}

void Renderer::applyCameraTransform(Camera& camera, Vector2<>& point) {
	point.x -= camera.getPosition().x;
	point.y -= camera.getPosition().y;
}

void Renderer::applyScreenTransform(Vector2<>& point) {
	point.x *= WINDOW_WIDTH / RENDER_WIDTH;
	point.y *= WINDOW_HEIGHT / RENDER_HEIGHT;
}

void Renderer::renderUIElement(GameObject& uiElement) {
	Camera staticCamera = Camera(); // UI does not depend on camera
	renderGameObject(uiElement, staticCamera);
}

void Renderer::renderUI(UI& ui) {
	for (GameObject uiElement : ui.getUIElements()) {
		renderUIElement(uiElement);
	}
}

// TODO: text class instead of string?
void Renderer::renderText(Vector2<> position, std::string text, TextOverMode textOverMode) {
	Vector2<> positionRounded = Vector2<>((int)position.x, (int)position.y);
	TTF_Font* font = resourceManager.loadFont("arial.ttf", 12);

	TTF_Text* ttfText = TTF_CreateText(textEngine, font, text.c_str(), text.size());

	int w, h;

	TTF_GetTextSize(ttfText, &w, &h);

#ifndef PIXEL_PERFECT
	Vector2<> textSize = Vector2<>(w, h);
	applyScreenTransform(textSize);
	w = textSize.x;
	h = textSize.y;
#endif

	positionRounded.x -= w / 2;
	if (textOverMode == TEXT_ON_TOP)
		positionRounded.y -= h;
	else if (textOverMode == TEXT_CENTERED)
		positionRounded.y -= h/2;

	SDL_Color White = { 200, 200, 200 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), 0, White);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
	SDL_FRect dstRect{ positionRounded.x, positionRounded.y, w, h};
	SDL_RenderTexture(sdlRenderer, texture, NULL, &dstRect);

	SDL_DestroySurface(surface);
	SDL_DestroyTexture(texture);
	TTF_DestroyText(ttfText);
}

void Renderer::renderTexture(Vector2<> position, Vector2<> size, Texture* textureToRender, Camera& camera) {
	SDL_FRect srcRect;
	SDL_Texture* texture = resourceManager.loadTexture(sdlRenderer, *textureToRender, &srcRect);
	if (textureToRender->getSource() == "" || texture == nullptr) {
		// If there is no texture source file, it might be a solid color texture
		int color[4];
		textureToRender->getColor(color);
		if (color[0] == -1 || color[1] == -1 || color[2] == -1 || color[3] == -1) {
			SDL_Log("Invalid texture without source file");
			return;
		}

		SDL_FRect colorRect = { position.x, position.y, size.x, size.y };

		SDL_SetRenderDrawColor(sdlRenderer, color[0], color[1], color[2], color[3]);
		SDL_RenderFillRect(sdlRenderer, &colorRect);
		return;
	}
	SDL_FRect dstRect;
	dstRect.x = position.x;
	dstRect.y = position.y;
	dstRect.w = size.x;
	dstRect.h = size.y;
	applyCameraTransform(camera, dstRect);
#ifndef PIXEL_PERFECT
	applyScreenTransform(dstRect);
#endif
	bool renderTextureSuccess = false;
	if (textureToRender->isMirrored()) {
		SDL_FPoint spriteCenter = { textureToRender->getSize().x / 2, textureToRender->getSize().y / 2 };
		renderTextureSuccess = SDL_RenderTextureRotated(sdlRenderer, texture, &srcRect, &dstRect, 0, &spriteCenter, SDL_FLIP_HORIZONTAL);
	}
	else {
		renderTextureSuccess = SDL_RenderTexture(sdlRenderer, texture, &srcRect, &dstRect);
	}
	if (!renderTextureSuccess) {
		SDL_Log("Failed to render texture: %s.\nPath: %s%s", SDL_GetError(), TEXTURES_DIRECTORY, textureToRender->getSource().c_str());
	}
	//SDL_SetRenderDrawColor(sdlRenderer, 0xff * object->getPosition().x, 0xff * object->getPosition().x, 0xff * object->getPosition().x, 0xff);
	//SDL_RenderFillRect(sdlRenderer, &dstRect);
}

void Renderer::renderGameObject(GameObject& gameObject, Camera& camera) {
	if (!gameObject.isVisible()) {
		return;
	}


	//SDL_FRect rect;
	//rect.x = object.getPosition().x;
	//rect.y = object.getPosition().y;
	//rect.w = object.getSize().x;
	//rect.h = object.getSize().y;
	//SDL_SetRenderDrawColor(sdlRenderer, 0, 0xff, 0, 0xff);
	//SDL_RenderFillRect(sdlRenderer, &rect);

	Texture* textureToRender;
	if (gameObject.getAnimation() != nullptr) {
		// We prioritize animation over static texture
		Animation* animation = gameObject.getAnimation();

		int textureIndex = animation->currentFrame / animation->getFrameRate();
		textureToRender = &animation->getFrames()[textureIndex];

		if (animation->currentFrame < animation->getFrames().size() * animation->getFrameRate() - 1) {
			animation->currentFrame++;
		}
		else if (animation->doesLoop()) {
			animation->currentFrame = 0;
		}
	}
	else {
		// There is no animation, we render the static texture
		textureToRender = gameObject.getTexture();
	}
	if (textureToRender != nullptr) {
		renderTexture(gameObject.getPosition(), gameObject.getSize(), textureToRender, camera);
	}

	if (gameObject.getTextOver() != "") {
		Vector2<> textPosition;
		if (gameObject.getTextOverMode() == TEXT_ON_TOP) {
			textPosition = gameObject.getPosition() + Vector2<>(gameObject.getSize().x / 2, 0);
		}
		else if (gameObject.getTextOverMode() == TEXT_CENTERED) {
			textPosition = gameObject.getPosition() + Vector2<>(gameObject.getSize().x / 2, gameObject.getSize().y / 2);
		}
		
		//Vector2<> textPosition = gameObject.getPosition() + Vector2<>(gameObject.getSize().x / 2, 0);
		applyCameraTransform(camera, textPosition);
#ifndef PIXEL_PERFECT
		applyScreenTransform(textPosition);
#endif
		renderText(textPosition, gameObject.getTextOver(), gameObject.getTextOverMode());
	}
}

void Renderer::checkOverlaps(GameObject& gameObject1, GameObject& gameObject2) {
	std::map<GameObject*, Event>::iterator it = gameObject1.getOverlapEvents().find(&gameObject2);
	if (it == gameObject1.getOverlapEvents().end()) {
		return;
	}

	SDL_Rect rectObj1 = { gameObject1.getPosition().x, gameObject1.getPosition().y, gameObject1.getSize().x, gameObject1.getSize().y };
	SDL_Rect rectObj2 = { gameObject2.getPosition().x, gameObject2.getPosition().y, gameObject2.getSize().x, gameObject2.getSize().y };
	int currentOverlapping = -1;
	for (int i = 0; i < currentlyOverlapping.size(); i++) {
		auto overlap = currentlyOverlapping[i];
		if (overlap[0] == &gameObject1 && overlap[1] == &gameObject2) {
			currentOverlapping = i;
		}
	}

	if (SDL_HasRectIntersection(&rectObj1, &rectObj2)) {
		if (currentOverlapping == -1) {
			it->second.execute();
			std::array<GameObject*, 2> overlapping;
			overlapping[0] = &gameObject1;
			overlapping[1] = &gameObject2;
			currentlyOverlapping.push_back(overlapping);
		}
	}
	else {
		if (currentOverlapping != -1) {
			currentlyOverlapping.erase(currentlyOverlapping.begin() + currentOverlapping);
		}
	}
}


void Renderer::renderDebugElements(Scene& scene) {
	SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0, 0, 0xff);

	for (Vector2<> point : scene.getDebugPoints()) {
		applyCameraTransform(scene.getCamera(), point);
#ifndef PIXEL_PERFECT
		applyScreenTransform(point);
#endif
		SDL_RenderPoint(sdlRenderer, point.x, point.y);
	}

	for (auto line : scene.getDebugLines()) {
		applyCameraTransform(scene.getCamera(), line[0]);
		applyCameraTransform(scene.getCamera(), line[1]);
#ifndef PIXEL_PERFECT
		applyScreenTransform(line[0]);
		applyScreenTransform(line[1]);
#endif
		SDL_RenderLine(sdlRenderer, line[0].x, line[0].y, line[1].x, line[1].y);
	}
}


void Renderer::render(Scene& scene, UI& ui) {


	auto now = std::chrono::system_clock::now();
	auto lastRenderTimeDiff = now - lastRenderTime;
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(lastRenderTimeDiff).count();

	auto msUntilNextFrame = (1000 / FPS) - ms;

	if (msUntilNextFrame > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(msUntilNextFrame));
	}

#ifdef PIXEL_PERFECT
	SDL_Texture* lowResScreen = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, RENDER_WIDTH, RENDER_HEIGHT);
	SDL_SetTextureScaleMode(lowResScreen, SDL_SCALEMODE_NEAREST);
	SDL_SetRenderTarget(sdlRenderer, lowResScreen);
#endif

	for (GameObject* object : scene.getGameObjects()) {
		if (object == nullptr) {
			SDL_Log("Invalid gameObject.");
			continue;
		}

		for (GameObject* object2 : scene.getGameObjects()) {
			if (object2 == nullptr) {
				SDL_Log("Invalid gameObject.");
				continue;
			}
			// Check overlaps to call events

			checkOverlaps(*object, *object2);
		}

		// Render the object
		renderGameObject(*object, scene.getCamera());
	}

#ifdef _DEBUG
	renderDebugElements(scene);
#endif

	renderUI(ui);

#ifdef PIXEL_PERFECT
	SDL_SetRenderTarget(sdlRenderer, NULL);
	SDL_RenderTexture(sdlRenderer, lowResScreen, NULL, NULL);
#endif

	SDL_RenderPresent(sdlRenderer);

#ifdef PIXEL_PERFECT
	SDL_DestroyTexture(lowResScreen);
#endif
	lastRenderTime = now;
}