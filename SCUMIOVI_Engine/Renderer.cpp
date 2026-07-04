#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "Common.hpp"
#include "Event.hpp"
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include <unordered_map>
#include <string>

Renderer::Renderer() : resourceManager(ResourceManager::getInstance()) {
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
		SDL_Log("SDL_Init error %s", SDL_GetError());
		validState = false;
		return;
	}

	sdlWindow = SDL_CreateWindow("SCUMIOVI", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (sdlWindow == NULL) {
		SDL_Log("SDL_CreateWindow error %s", SDL_GetError());
		validState = false;
		return;
	}

	sdlRenderer = SDL_CreateRenderer(sdlWindow, NULL);
	if (sdlRenderer == NULL) {
		SDL_Log("SDL_CreateRenderer error %s", SDL_GetError());
		validState = false;
		return;
	}

	textEngine = TTF_CreateRendererTextEngine(sdlRenderer);
	if (textEngine == NULL) {
		SDL_Log("TTF_CreateRendererTextEngine error %s", SDL_GetError());
		validState = false;
	}
}

Renderer::~Renderer() {
	if (sdlRenderer) SDL_DestroyRenderer(sdlRenderer);
	if (sdlWindow) SDL_DestroyWindow(sdlWindow);
	if (textEngine) TTF_DestroyRendererTextEngine(textEngine);
}

void Renderer::applyCameraTransform(Camera& camera, SDL_FRect& rect) {
	rect.x -= camera.getPosition().x;
	rect.y -= camera.getPosition().y;
}

void Renderer::applyCameraTransform(Camera& camera, Vector2<>& point) {
	point.x -= camera.getPosition().x;
	point.y -= camera.getPosition().y;
}

void Renderer::applyScreenTransform(Vector2<>& point, Vector2<> renderFrameSize) {
	point.x *= WINDOW_WIDTH / renderFrameSize.x;
	point.y *= WINDOW_HEIGHT / renderFrameSize.y;
}

void Renderer::applyScreenTransform(SDL_FRect& rect, Vector2<> renderFrameSize) {
	rect.x *= WINDOW_WIDTH / renderFrameSize.x;
	rect.y *= WINDOW_HEIGHT / renderFrameSize.y;
	rect.w *= WINDOW_WIDTH / renderFrameSize.x;
	rect.h *= WINDOW_HEIGHT / renderFrameSize.y;
}

void Renderer::renderUIElement(GameObject& uiElement) {
	renderGameObject(uiElement, zeroCamera);
}

void Renderer::renderUI(UI& ui) {
	if (ui.getLayout() == nullptr)
		return;
	auto elements = ui.getUIElements();
	for (const auto& uiElement : elements) {
		renderUIElement(*uiElement);
	}
}

std::string Renderer::generateTextureKey(TTF_Font* font, const std::string& textString, const int color[3], bool hasOutline) {
	return std::to_string(reinterpret_cast<uintptr_t>(font)) + ":" + textString + ":" +
		std::to_string(color[0]) + "," + std::to_string(color[1]) + "," + std::to_string(color[2]) + ":" + std::to_string(hasOutline);
}

#define OUTLINE_SIZE 1

void Renderer::renderTTFText(Vector2<> pos, Vector2<> size, TTF_Font* font, const std::string& textString, const int color[3], bool hasOutline) {
	static std::unordered_map<std::string, SDL_Texture*> textureCache;

	SDL_Color sdlColor = { static_cast<Uint8>(color[0]), static_cast<Uint8>(color[1]), static_cast<Uint8>(color[2]) };

	std::string key = generateTextureKey(font, textString, color, hasOutline);
	SDL_Texture* texture = nullptr;

	auto it = textureCache.find(key);
	if (it != textureCache.end()) {
		texture = it->second;
	}
	else {
		if (hasOutline)
			TTF_SetFontOutline(font, OUTLINE_SIZE);
		SDL_Surface* surface = TTF_RenderText_Solid_Wrapped(font, textString.c_str(), 0, sdlColor, 0);
		if (hasOutline)
			TTF_SetFontOutline(font, 0);
		if (surface) {
			texture = SDL_CreateTextureFromSurface(sdlRenderer, surface);
			SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
			SDL_DestroySurface(surface);
			textureCache[key] = texture;
		}
	}

	if (texture) {
		SDL_FRect dstRect{ pos.x, pos.y, size.x, size.y };
		SDL_RenderTexture(sdlRenderer, texture, NULL, &dstRect);
	}
}

void Renderer::renderText(Vector2<> position, Text& text, Camera& camera, bool hasOutline) {
	TTF_Font* font = resourceManager.loadFont("arial.ttf", text.getSize());

	if (text.hasShadow()) {
		int color[] = { 0x40, 0x0, 0x40 };
		Text shadowText = Text(text.toString(), text.getSize(), color, false, text.getTextOverMode(), false);
		renderText(position + Vector2<>(1, 1), shadowText, camera);
	}

	if (text.hasOutline()) {
		int blackColor[] = { 0, 0, 0 };
		Text outlineText = Text(text.toString(), text.getSize(), blackColor, false, text.getTextOverMode(), false);
		renderText(position+Vector2<>(0,1), outlineText, camera, true);
	}

	Vector2<> positionRounded = Vector2<>((int)position.x, (int)position.y);
	TTF_Text* ttfText = TTF_CreateText(textEngine, font, text.toString().c_str(), (size_t)text.toString().size());
	int w, h;
	TTF_GetTextSize(ttfText, &w, &h);
	if (hasOutline) {
		w += OUTLINE_SIZE*2;
		h += OUTLINE_SIZE*2;
	}
#ifndef PIXEL_PERFECT
	Vector2<> textSize = Vector2<>((float)w, (float)h);
	applyScreenTransform(textSize, camera.getSize());
	w = (int)textSize.x;
	h = (int)textSize.y;
#endif
	if (text.getTextOverMode() != TEXT_TO_LEFT)
		positionRounded.x -= w / 2;
	if (text.getTextOverMode() == TEXT_ON_TOP)
		positionRounded.y -= (float)h;
	else if (text.getTextOverMode() == TEXT_CENTERED || text.getTextOverMode() == TEXT_TO_LEFT)
		positionRounded.y -= (float)h / 2;
	int color[4];
	text.getColor(color);

	renderTTFText(positionRounded, Vector2<>((float)w,(float)h), font, text.toString(), color, hasOutline);

	TTF_DestroyText(ttfText);
}

void Renderer::renderTexture(Vector2<> position, Vector2<> size, std::shared_ptr<Texture> textureToRender, Camera& camera) {
	if (textureToRender == nullptr) return;
	
	SDL_FRect srcRect;
	SDL_Texture* texture = resourceManager.loadTexture(sdlRenderer, *textureToRender, &srcRect);
	if (textureToRender->getSource() == "" || texture == nullptr) {
		int color[4];
		textureToRender->getColor(color);
		if (color[0] == -1 || color[1] == -1 || color[2] == -1 || color[3] == -1) {
			return;
		}

		SDL_FRect colorRect = { position.x, position.y, size.x, size.y };
		applyCameraTransform(camera, colorRect);
#ifndef PIXEL_PERFECT
		applyScreenTransform(colorRect, camera.getSize());
#endif

		SDL_SetRenderDrawColor(sdlRenderer, (Uint8)color[0], (Uint8)color[1], (Uint8)color[2], (Uint8)color[3]);
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
	applyScreenTransform(dstRect, camera.getSize());
#endif
	
	bool renderTextureSuccess = false;
	if (textureToRender->isMirrored()) {
		SDL_FPoint spriteCenter = { (float)textureToRender->getSize().x / 2, (float)textureToRender->getSize().y / 2 };
		renderTextureSuccess = SDL_RenderTextureRotated(sdlRenderer, texture, &srcRect, &dstRect, 0, &spriteCenter, SDL_FLIP_HORIZONTAL);
	}
	else {
		renderTextureSuccess = SDL_RenderTexture(sdlRenderer, texture, &srcRect, &dstRect);
	}
	if (!renderTextureSuccess) {
		SDL_Log("Failed to render texture: %s.\nPath: %s%s", SDL_GetError(), ResourceManager::getTexturesDirectory().c_str(), textureToRender->getSource().c_str());
	}
}

void Renderer::renderGameObject(GameObject& gameObject, Camera& camera) {
	if (!gameObject.isVisible()) {
		return;
	}

	std::shared_ptr<Texture> textureToRender = nullptr;
	auto animation = gameObject.getAnimation();
	
	if (animation != nullptr) {
		int textureIndex = animation->currentFrame / animation->getFrameRate();
		auto& frames = animation->getFrames();
		if (textureIndex < frames.size()) {
			// Frames are currently stored as vector<Texture> in Animation.
			// Animation.hpp: std::vector<Texture> frames;
			// I should probably update Animation to hold shared_ptr<Texture> too?
			// For now, I'll take a pointer to the frame if it's value-based, but 
			// wait, our renderTexture takes shared_ptr.
			
			// Let's create a temporary shared_ptr for the frame if needed, 
			// though it's better to update Animation class too.
			textureToRender = std::make_shared<Texture>(frames[textureIndex]);
		}

		if (animation->currentFrame < frames.size() * animation->getFrameRate() - 1) {
			animation->currentFrame++;
		}
		else if (animation->doesLoop()) {
			animation->currentFrame = 0;
		}
	}
	else {
		textureToRender = gameObject.getTexture();
	}
	
	if (textureToRender != nullptr) {
		renderTexture(gameObject.getPosition(), gameObject.getSize(), textureToRender, camera);
	}
	
	Text& textOverObject = gameObject.getTextOver();

	if (textOverObject.toString() != "") {
		Vector2<> textPosition;
		if (textOverObject.getTextOverMode() == TEXT_ON_TOP) {
			textPosition = gameObject.getPosition() + Vector2<>(gameObject.getSize().x / 2, 0);
		}
		else if (textOverObject.getTextOverMode() == TEXT_CENTERED) {
			textPosition = gameObject.getPosition() + Vector2<>(gameObject.getSize().x / 2, gameObject.getSize().y / 2);
		}
		else if (textOverObject.getTextOverMode() == TEXT_TO_LEFT) {
			textPosition = gameObject.getPosition() + Vector2<>(0, gameObject.getSize().y / 2);
		}
		
		applyCameraTransform(camera, textPosition);
#ifndef PIXEL_PERFECT
		applyScreenTransform(textPosition, camera.getSize());
#endif

		renderText(textPosition, gameObject.getTextOver(), camera);
	}
}

void Renderer::checkOverlaps(std::shared_ptr<GameObject> gameObject1, std::shared_ptr<GameObject> gameObject2) {
	auto& overlapEvents = gameObject1->getOverlapEvents();
	auto it = overlapEvents.find(gameObject2);
	if (it == overlapEvents.end()) {
		return;
	}

	SDL_Rect rectObj1 = { (int)gameObject1->getPosition().x, (int)gameObject1->getPosition().y, (int)gameObject1->getSize().x, (int)gameObject1->getSize().y };
	SDL_Rect rectObj2 = { (int)gameObject2->getPosition().x, (int)gameObject2->getPosition().y, (int)gameObject2->getSize().x, (int)gameObject2->getSize().y };
	
	int currentOverlappingIndex = -1;
	for (int i = 0; i < (int)currentlyOverlapping.size(); i++) {
		auto& overlap = currentlyOverlapping[i];
		if (overlap[0] == gameObject1 && overlap[1] == gameObject2) {
			currentOverlappingIndex = i;
			break;
		}
	}

	if (SDL_HasRectIntersection(&rectObj1, &rectObj2)) {
		if (currentOverlappingIndex == -1) {
			it->second.execute();
			std::array<std::shared_ptr<GameObject>, 2> overlapping;
			overlapping[0] = gameObject1;
			overlapping[1] = gameObject2;
			currentlyOverlapping.push_back(overlapping);
		}
	}
	else {
		if (currentOverlappingIndex != -1) {
			currentlyOverlapping.erase(currentlyOverlapping.begin() + currentOverlappingIndex);
		}
	}
}

void Renderer::renderDebugElements(Scene& scene) {
	SDL_SetRenderDrawColor(sdlRenderer, 0xff, 0, 0, 0xff);

	for (Vector2<> point : scene.getDebugPoints()) {
		applyCameraTransform(scene.getCamera(), point);
#ifndef PIXEL_PERFECT
		applyScreenTransform(point, scene.getCamera().getSize());
#endif
		SDL_RenderPoint(sdlRenderer, point.x, point.y);
	}

	for (auto& line : scene.getDebugLines()) {
		Vector2<> p1 = line[0];
		Vector2<> p2 = line[1];
		applyCameraTransform(scene.getCamera(), p1);
		applyCameraTransform(scene.getCamera(), p2);
#ifndef PIXEL_PERFECT
		applyScreenTransform(p1, scene.getCamera().getSize());
		applyScreenTransform(p2, scene.getCamera().getSize());
#endif
		SDL_RenderLine(sdlRenderer, p1.x, p1.y, p2.x, p2.y);
	}
}

void Renderer::setCustomMouse(std::shared_ptr<Texture> mouseTexture, Vector2<> mouseHotspot) {
	if (customCursor != nullptr) {
		SDL_DestroyCursor(customCursor);
		customCursor = nullptr;
	}

	if (mouseTexture) {
		SDL_Surface* cursorSurface = resourceManager.loadSurface(*mouseTexture);
		if (!cursorSurface) {
			SDL_Log("SDL_LoadBMP error %s", SDL_GetError());
		}

		if (mouseHotspot.x == -1) {
			mouseHotspot.x = (float)mouseTexture->getSize().x / 2;
		}
		if (mouseHotspot.y == -1) {
			mouseHotspot.y = (float)mouseTexture->getSize().y / 2;
		}

		this->mouseTexture = mouseTexture;
		this->mouseHotspot = mouseHotspot;

#ifndef PIXEL_PERFECT_CURSOR
		customCursor = SDL_CreateColorCursor(cursorSurface, (int)mouseHotspot.x, (int)mouseHotspot.y);
		if (!customCursor) {
			SDL_Log("SDL_CreateColorCursor error %s", SDL_GetError());
		}
		SDL_SetCursor(customCursor);
#else
		SDL_HideCursor();
#endif
	}
}

void Renderer::render(Scene& scene, UI& ui) {
	if (!validState) return;
#ifdef PIXEL_PERFECT
	SDL_Texture* lowResScreen = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, (int)scene.getCamera().getSize().x, (int)scene.getCamera().getSize().y);
	SDL_SetTextureScaleMode(lowResScreen, SDL_SCALEMODE_NEAREST);
	SDL_SetRenderTarget(sdlRenderer, lowResScreen);
#endif

	auto gameObjects = scene.getGameObjects();
	for (const auto& object : gameObjects) {
		if (object == nullptr) continue;

		for (const auto& object2 : gameObjects) {
			if (object2 == nullptr || object == object2) continue;
			checkOverlaps(object, object2);
		}

		renderGameObject(*object, scene.getCamera());
	}

#ifdef _DEBUG
	renderDebugElements(scene);
#endif

	renderUI(ui);

#ifdef PIXEL_PERFECT_CURSOR
	if (mouseTexture != nullptr) {
		float xMouse;
		float yMouse;
		SDL_GetMouseState(&xMouse, &yMouse);

		xMouse /= WINDOW_WIDTH / scene.getCamera().getSize().x;
		yMouse /= WINDOW_HEIGHT / scene.getCamera().getSize().y;

		xMouse -= mouseHotspot.x;
		yMouse -= mouseHotspot.y;

		xMouse = (float)((int)xMouse);
		yMouse = (float)((int)yMouse);

		Camera c; 
		renderTexture(Vector2<>(xMouse, yMouse), Vector2<float>((float)mouseTexture->getSize().x, (float)mouseTexture->getSize().y), mouseTexture, c);
	}
#endif

#ifdef PIXEL_PERFECT
	SDL_SetRenderTarget(sdlRenderer, NULL);
	SDL_RenderTexture(sdlRenderer, lowResScreen, NULL, NULL);
	SDL_DestroyTexture(lowResScreen);
#endif

	renderFadeOverlay();

	SDL_RenderPresent(sdlRenderer);
}

void Renderer::renderFadeOverlay() {
	if (fadeAlpha <= 0.0f) return;
	SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, (Uint8)(fadeAlpha * 255.0f));
	SDL_RenderFillRect(sdlRenderer, NULL);
}

void Renderer::fadeToBlack(float duration, Event callback) {
	if (duration <= 0.0f) {
		fadeAlpha = 1.0f;
		callback.execute();
		return;
	}
	fadeAlpha = 0.0f;
	fadeTarget = 1.0f;
	fadeSpeed = 1.0f / duration;
	fadeCompleteEvent = callback;
	fading = true;
}

void Renderer::fadeFromBlack(float duration, Event callback) {
	if (duration <= 0.0f) {
		fadeAlpha = 0.0f;
		callback.execute();
		return;
	}
	fadeAlpha = 1.0f;
	fadeTarget = 0.0f;
	fadeSpeed = 1.0f / duration;
	fadeCompleteEvent = callback;
	fading = true;
}

bool Renderer::isFading() const {
	return fading;
}

void Renderer::updateFade(float dt) {
	if (!fading) return;

	if (fadeAlpha < fadeTarget) {
		fadeAlpha += fadeSpeed * dt;
		if (fadeAlpha >= fadeTarget) {
			fadeAlpha = fadeTarget;
			fading = false;
			fadeCompleteEvent.execute();
			fadeCompleteEvent = Event();
		}
	}
	else if (fadeAlpha > fadeTarget) {
		fadeAlpha -= fadeSpeed * dt;
		if (fadeAlpha <= fadeTarget) {
			fadeAlpha = fadeTarget;
			fading = false;
			fadeCompleteEvent.execute();
			fadeCompleteEvent = Event();
		}
	}
}