#include "UIAction.hpp"
#include "Game.hpp"
#include "UILayout.hpp"

#include <SDL3/SDL.h>

using nlohmann::json;

Event resolveUIAction(const json& actionJson) {
	if (actionJson.is_string()) {
		return Game::getInstance().scriptEvent(actionJson.get<std::string>());
	}
	if (!actionJson.is_object() || !actionJson.contains("action")) {
		return Event();
	}
	std::string action = actionJson.value("action", std::string());

	if (action == "script") {
		return Game::getInstance().scriptEvent(actionJson.value("name", std::string()));
	}
	if (action == "openLayout") {
		std::string name = actionJson.value("name", std::string());
		return Event(std::function<void(EventInfo)>([name](EventInfo e) {
			e.game.getUI().setLayout(name);
		}));
	}
	if (action == "openOverlay") {
		std::string name = actionJson.value("name", std::string());
		bool pause = actionJson.value("pauseGame", true);
		return Event(std::function<void(EventInfo)>([name, pause](EventInfo e) {
			e.game.getUI().openOverlay(name, pause);
		}));
	}
	if (action == "toggleOverlay") {
		// Opens the named overlay, or closes it if it's already the active one
		// — the natural behavior for a single key (e.g. Escape) that both opens
		// and dismisses the same menu.
		std::string name = actionJson.value("name", std::string());
		bool pause = actionJson.value("pauseGame", true);
		return Event(std::function<void(EventInfo)>([name, pause](EventInfo e) {
			UI& ui = e.game.getUI();
			auto* overlay = ui.getOverlay();
			if (overlay && overlay->getName() == name) {
				ui.closeOverlay();
			} else {
				ui.openOverlay(name, pause);
			}
		}));
	}
	if (action == "closeOverlay") {
		return Event(std::function<void(EventInfo)>([](EventInfo e) {
			e.game.getUI().closeOverlay();
		}));
	}
	if (action == "setLocale") {
		std::string locale = actionJson.value("locale", std::string("en"));
		return Event(std::function<void(EventInfo)>([locale](EventInfo e) {
			e.game.setLocale(locale);
		}));
	}
	if (action == "setVerb") {
		std::string verbName = actionJson.value("name", std::string());
		return Event(std::function<void(EventInfo)>([verbName](EventInfo e) {
			auto player = e.game.getCurrentPlayer();
			if (player) player->setVerb(verbName);
		}));
	}
	if (action == "switchPlayer") {
		// Switches to a specific party member by name; see Game::switchToPlayer.
		std::string name = actionJson.value("name", std::string());
		float fadeDuration = actionJson.value("fadeDuration", 0.0f);
		return Event(std::function<void(EventInfo)>([name, fadeDuration](EventInfo e) {
			e.game.switchToPlayer(name, fadeDuration);
		}));
	}
	if (action == "switchToNextPlayer") {
		float fadeDuration = actionJson.value("fadeDuration", 0.0f);
		return Event(std::function<void(EventInfo)>([fadeDuration](EventInfo e) {
			e.game.switchToNextPlayer(fadeDuration);
		}));
	}
	if (action == "switchToPreviousPlayer") {
		float fadeDuration = actionJson.value("fadeDuration", 0.0f);
		return Event(std::function<void(EventInfo)>([fadeDuration](EventInfo e) {
			e.game.switchToPreviousPlayer(fadeDuration);
		}));
	}
	if (action == "save") {
		int slot = actionJson.value("slot", 0);
		return Event(std::function<void(EventInfo)>([slot](EventInfo e) {
			e.game.saveGame(slot);
		}));
	}
	if (action == "load") {
		int slot = actionJson.value("slot", 0);
		return Event(std::function<void(EventInfo)>([slot](EventInfo e) {
			e.game.loadGame(slot);
		}));
	}
	if (action == "quit") {
		return Event(std::function<void(EventInfo)>([](EventInfo e) {
			e.game.quitGame();
		}));
	}

	SDL_Log("resolveUIAction: unknown action '%s'", action.c_str());
	return Event();
}
