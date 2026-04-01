#include "Renderer.hpp"

#include "Common.hpp"

#include <SDL3_ttf/SDL_ttf.h>

Renderer::Renderer() : resourceManager(ResourceManager::getInstance()) {
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
	// UI does not depend on camera position, so I use the dummy zeroCamera here
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

	// TODO: outline and shadow custom color

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
	TTF_Text* ttfText = TTF_CreateText(textEngine, font, text.toString().c_str(), text.toString().size());
	int w, h;
	TTF_GetTextSize(ttfText, &w, &h);
	if (hasOutline) {
		w += OUTLINE_SIZE*2;
		h += OUTLINE_SIZE*2;
	}
#ifndef PIXEL_PERFECT
	Vector2<> textSize = Vector2<>(w, h);
	applyScreenTransform(textSize, camera.getSize());
	w = textSize.x;
	h = textSize.y;
#endif
	if (text.getTextOverMode() != TEXT_TO_LEFT)
		positionRounded.x -= w / 2;
	if (text.getTextOverMode() == TEXT_ON_TOP)
		positionRounded.y -= h;
	else if (text.getTextOverMode() == TEXT_CENTERED || text.getTextOverMode() == TEXT_TO_LEFT)
		positionRounded.y -= h / 2;
	int color[4];
	text.getColor(color);

	renderTTFText(positionRounded, Vector2<>(w,h), font, text.toString(), color, hasOutline);


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
			//SDL_Log("Invalid texture without source file");
			return;
		}

		SDL_FRect colorRect = { position.x, position.y, size.x, size.y };

		applyCameraTransform(camera, colorRect);
#ifndef PIXEL_PERFECT
		applyScreenTransform(colorRect, camera.getSize());
#endif

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
	applyScreenTransform(dstRect, camera.getSize());
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
		
		//Vector2<> textPosition = gameObject.getPosition() + Vector2<>(gameObject.getSize().x / 2, 0);
		applyCameraTransform(camera, textPosition);
#ifndef PIXEL_PERFECT
		applyScreenTransform(textPosition, camera.getSize());
#endif

		renderText(textPosition, gameObject.getTextOver(), camera);
	}
}

void Renderer::checkOverlaps(std::shared_ptr<GameObject> gameObject1, std::shared_ptr<GameObject> gameObject2) {
	std::map<std::shared_ptr<GameObject>, Event>::iterator it = gameObject1->getOverlapEvents().find(gameObject2);
	if (it == gameObject1->getOverlapEvents().end()) {
		return;
	}

	SDL_Rect rectObj1 = { (int)gameObject1->getPosition().x, (int)gameObject1->getPosition().y, (int)gameObject1->getSize().x, (int)gameObject1->getSize().y };
	SDL_Rect rectObj2 = { (int)gameObject2->getPosition().x, (int)gameObject2->getPosition().y, (int)gameObject2->getSize().x, (int)gameObject2->getSize().y };
	int currentOverlappingIndex = -1;
	for (int i = 0; i < currentlyOverlapping.size(); i++) {
		auto overlap = currentlyOverlapping[i];
		if (overlap[0] == gameObject1 && overlap[1] == gameObject2) {
			currentOverlappingIndex = i;
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

	for (auto line : scene.getDebugLines()) {
		applyCameraTransform(scene.getCamera(), line[0]);
		applyCameraTransform(scene.getCamera(), line[1]);
#ifndef PIXEL_PERFECT
		applyScreenTransform(line[0], scene.getCamera().getSize());
		applyScreenTransform(line[1], scene.getCamera().getSize());
#endif
		SDL_RenderLine(sdlRenderer, line[0].x, line[0].y, line[1].x, line[1].y);
	}
}

void Renderer::setCustomMouse(Texture& mouseTexture, Vector2<> mouseHotspot) {
	if (customCursor != nullptr) {
		SDL_DestroyCursor(customCursor);
	}

	SDL_Surface* cursorSurface = resourceManager.loadSurface(mouseTexture); // Change to your cursor image path
	if (!cursorSurface) {
		SDL_Log("SDL_LoadBMP error %s", SDL_GetError());
	}

	// Vector2<>(-1,-1) is used to represent that the hotspot is in the center
	if (mouseHotspot.x == -1) {
		mouseHotspot.x = mouseTexture.getSize().x / 2;
	}
	if (mouseHotspot.y == -1) {
		mouseHotspot.y = mouseTexture.getSize().y / 2;
	}

	this->mouseTexture = &mouseTexture;
	this->mouseHotspot = mouseHotspot;

#ifndef PIXEL_PERFECT_CURSOR
	// Create a custom cursor from the surface
	SDL_Cursor* customCursor = SDL_CreateColorCursor(cursorSurface, mouseHotspot.x, mouseHotspot.y); // (0, 0) is the hotspot
	if (!customCursor) {
		SDL_Log("SDL_CreateColorCursor error %s", SDL_GetError());
	}

	// Set the custom cursor
	SDL_SetCursor(customCursor);
#else
	SDL_HideCursor();
#endif
}

void Renderer::render(Scene& scene, UI& ui) {
#ifdef PIXEL_PERFECT
	SDL_Texture* lowResScreen = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_TARGET, scene.getCamera().getSize().x, scene.getCamera().getSize().y);
	SDL_SetTextureScaleMode(lowResScreen, SDL_SCALEMODE_NEAREST);
	SDL_SetRenderTarget(sdlRenderer, lowResScreen);
#endif

	auto gameObjects = scene.getGameObjects();
	for (const auto& object : gameObjects) {
		if (object == nullptr) {
			SDL_Log("Invalid gameObject.");
			continue;
		}

		for (const auto& object2 : gameObjects) {
			if (object2 == nullptr) {
				SDL_Log("Invalid gameObject.");
				continue;
			}
			// Check overlaps to call events

			checkOverlaps(object, object2);
		}

		// Render the object
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

		// Round position
		xMouse = (int)xMouse;
		yMouse = (int)yMouse;

		Camera c; // Camera at 0,0 (Cursor does not depend on camera position)
		renderTexture(Vector2<>(xMouse, yMouse), Vector2<float>(mouseTexture->getSize().x, mouseTexture->getSize().y), mouseTexture, c);
	}
#endif


#ifdef PIXEL_PERFECT
	SDL_SetRenderTarget(sdlRenderer, NULL);
	SDL_RenderTexture(sdlRenderer, lowResScreen, NULL, NULL);
#endif

	SDL_RenderPresent(sdlRenderer);

#ifdef PIXEL_PERFECT
	SDL_DestroyTexture(lowResScreen);
#endif
}