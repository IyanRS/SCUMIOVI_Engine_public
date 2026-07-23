#include "UILoader.hpp"

#include "Game.hpp"
#include "UILayout.hpp"
#include "UIElement.hpp"
#include "UIVerbGrid.hpp"
#include "UIInventoryGrid.hpp"
#include "UIInfoLabel.hpp"
#include "UIAction.hpp"
#include "Localization.hpp"
#include "json.hpp"

#include <SDL3/SDL.h>
#include <fstream>
#include <filesystem>

using nlohmann::json;

namespace {
	std::string generateName(const std::string& type) {
		static int counter = 0;
		return type + "_" + std::to_string(counter++);
	}

	Vector2<> readVec2(const json& j, const std::string& key, Vector2<> def) {
		if (!j.contains(key)) return def;
		const json& arr = j.at(key);
		if (!arr.is_array() || arr.size() < 2) return def;
		return Vector2<>(arr[0].get<float>(), arr[1].get<float>());
	}

	void readColor(const json& j, const std::string& key, int outColor[4], int defR, int defG, int defB, int defA) {
		outColor[0] = defR; outColor[1] = defG; outColor[2] = defB; outColor[3] = defA;
		if (!j.contains(key)) return;
		const json& arr = j.at(key);
		if (!arr.is_array() || arr.empty()) return;
		if (arr.size() > 0) outColor[0] = arr[0].get<int>();
		if (arr.size() > 1) outColor[1] = arr[1].get<int>();
		if (arr.size() > 2) outColor[2] = arr[2].get<int>();
		outColor[3] = arr.size() > 3 ? arr[3].get<int>() : 0xff;
	}

	UIAnchor parseAnchor(const std::string& s) {
		if (s == "top-left") return UIAnchor::TopLeft;
		if (s == "top-center") return UIAnchor::TopCenter;
		if (s == "top-right") return UIAnchor::TopRight;
		if (s == "center-left") return UIAnchor::CenterLeft;
		if (s == "center") return UIAnchor::Center;
		if (s == "center-right") return UIAnchor::CenterRight;
		if (s == "bottom-left") return UIAnchor::BottomLeft;
		if (s == "bottom-center") return UIAnchor::BottomCenter;
		if (s == "bottom-right") return UIAnchor::BottomRight;
		return UIAnchor::TopLeft;
	}

	// Only the modes that actually exist on TextOverMode (Text.hpp) are mapped;
	// anything else (including unset) falls back to TEXT_ON_TOP.
	TextOverMode parseAlign(const std::string& s) {
		if (s == "center") return TEXT_CENTERED;
		if (s == "left") return TEXT_TO_LEFT;
		return TEXT_ON_TOP;
	}

	std::shared_ptr<Texture> resolveTexture(const json& j, const std::string& key) {
		if (!j.contains(key)) return nullptr;
		const json& val = j.at(key);
		if (val.is_string()) {
			return std::make_shared<Texture>("", val.get<std::string>());
		}
		if (val.is_array() && val.size() >= 3) {
			int color[4] = { val[0].get<int>(), val[1].get<int>(), val[2].get<int>(),
				val.size() > 3 ? val[3].get<int>() : 0xff };
			return std::make_shared<Texture>("", color);
		}
		return nullptr;
	}

	Text buildTextFormat(const json& j) {
		int color[4];
		readColor(j, "textColor", color, 255, 255, 255, 0xff);
		int size = j.value("textSize", 11);
		bool outline = j.value("textOutline", false);
		bool shadow = j.value("textShadow", false);
		TextOverMode align = parseAlign(j.value("textAlign", std::string("top")));
		return Text("", size, color, outline, align, shadow);
	}

	void applyAnchorAndVisibility(const json& j, std::shared_ptr<UIElement>& element) {
		if (j.contains("anchor")) {
			element->setAnchor(parseAnchor(j.value("anchor", std::string("top-left"))));
		}
		if (j.value("visible", true) == false) {
			element->setVisible(false);
		}
	}

	std::shared_ptr<GameObject> buildElement(const json& j);

	void buildChildren(const json& j, std::shared_ptr<UIElement> parent) {
		if (!j.contains("children")) return;
		for (auto& childJson : j.at("children")) {
			auto child = buildElement(childJson);
			if (child) parent->addChild(child);
		}
	}

	std::shared_ptr<GameObject> buildElement(const json& j) {
		std::string type = j.value("type", std::string("panel"));
		std::string name = j.value("name", generateName(type));
		Vector2<> position = readVec2(j, "position", Vector2<>(0, 0));
		Vector2<> size = readVec2(j, "size", Vector2<>(-1, -1));

		std::shared_ptr<UIElement> element;

		if (type == "image") {
			element = std::make_shared<UIImage>(name, position, size, resolveTexture(j, "texture"));
		}
		else if (type == "label") {
			// "saveSlot": N turns the label into a live save-timestamp
			// display for that slot (empty while the slot has no save).
			if (j.contains("saveSlot")) {
				element = std::make_shared<UISaveSlotLabel>(name, position, buildTextFormat(j),
					j.value("saveSlot", 0));
			}
			else {
				auto label = std::make_shared<UILabel>(name, position, buildTextFormat(j));
				label->setLocalizedText(j.value("text", std::string()));
				element = label;
			}
		}
		else if (type == "button") {
			Event onClick = j.contains("onClick") ? resolveUIAction(j.at("onClick")) : Event();
			Event onHover = j.contains("onHover") ? resolveUIAction(j.at("onHover")) : Event();
			auto button = std::make_shared<UIButton>(name, position, size, resolveTexture(j, "texture"),
				buildTextFormat(j), onClick, onHover);
			button->setLocalizedText(j.value("text", std::string()));
			if (j.value("hoverHighlight", false)) {
				button->setHoverHighlight(true);
			}
			element = button;
		}
		else if (type == "panel") {
			auto panel = std::make_shared<UIPanel>(name, position, size, resolveTexture(j, "texture"));
			buildChildren(j, panel);
			element = panel;
		}
		else if (type == "infoLabel") {
			// Self-refreshing label showing the current player's active verb
			// sentence (e.g. "Use key on door"); see UIInfoLabel.
			element = std::make_shared<UIInfoLabel>(name, position, buildTextFormat(j));
		}
		else {
			SDL_Log("UILoader: unknown element type '%s'", type.c_str());
			return nullptr;
		}

		applyAnchorAndVisibility(j, element);
		return element;
	}

	// verbGrid / inventoryGrid are built through UILayout::createVerbsMatrix /
	// createInventoryMatrix (which already own the label-width column sizing
	// and slot geometry) rather than through buildElement, so they can only
	// appear as top-level elements of a layout — the same constraint the
	// underlying C++ API already has today.
	void buildVerbGrid(UILayout& layout, const json& j) {
		Vector2<> position = readVec2(j, "position", Vector2<>(0, 0));
		auto texture = resolveTexture(j, "texture");

		std::vector<std::vector<std::shared_ptr<Verb>>> verbs;
		if (j.contains("verbs")) {
			Game& game = Game::getInstance();
			for (auto& rowJson : j.at("verbs")) {
				std::vector<std::shared_ptr<Verb>> row;
				for (auto& verbNameJson : rowJson) {
					std::string verbName = verbNameJson.get<std::string>();
					auto verb = game.getVerb(verbName);
					if (verb) row.push_back(verb);
					else SDL_Log("UILoader: unknown verb '%s' in verbGrid", verbName.c_str());
				}
				if (!row.empty()) verbs.push_back(row);
			}
		}
		if (!verbs.empty()) {
			layout.createVerbsMatrix(position, verbs, texture);
		}
		else {
			SDL_Log("UILoader: verbGrid has no valid \"verbs\" rows, skipped");
		}
	}

	void buildInventoryGrid(UILayout& layout, const json& j) {
		Vector2<> position = readVec2(j, "position", Vector2<>(0, 0));
		int columns = j.value("columns", 4);
		int rows = j.value("rows", 2);
		auto texture = resolveTexture(j, "texture");
		Event unused; // createInventoryMatrix's onUIUpdate parameter is not used internally
		layout.createInventoryMatrix(position, columns, rows, texture, unused);
	}
}

bool UILoader::loadLayout(Game& game, const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		SDL_Log("UILoader: could not open '%s'", path.c_str());
		return false;
	}

	json data;
	try {
		file >> data;
	}
	catch (const std::exception& ex) {
		SDL_Log("UILoader: failed to parse '%s': %s", path.c_str(), ex.what());
		return false;
	}

	std::string name = data.value("name", std::string());
	if (name.empty()) {
		SDL_Log("UILoader: '%s' is missing a top-level \"name\"", path.c_str());
		return false;
	}

	game.getUI().createLayout(name);
	UILayout& layout = game.getUI().getLayout(name);

	if (data.contains("elements")) {
		for (auto& elJson : data.at("elements")) {
			std::string type = elJson.value("type", std::string());
			if (type == "verbGrid") {
				buildVerbGrid(layout, elJson);
			}
			else if (type == "inventoryGrid") {
				buildInventoryGrid(layout, elJson);
			}
			else {
				auto element = buildElement(elJson);
				if (element) layout.addUIElement(element);
			}
		}
	}

	return true;
}

bool UILoader::loadLayoutsFromDirectory(Game& game, const std::string& directoryPath) {
	namespace fs = std::filesystem;
	if (!fs::exists(directoryPath) || !fs::is_directory(directoryPath)) {
		SDL_Log("UILoader: directory '%s' does not exist", directoryPath.c_str());
		return false;
	}

	bool allOk = true;
	for (auto& entry : fs::directory_iterator(directoryPath)) {
		if (entry.is_regular_file() && entry.path().extension() == ".json") {
			if (!loadLayout(game, entry.path().string())) {
				allOk = false;
			}
		}
	}
	return allOk;
}
