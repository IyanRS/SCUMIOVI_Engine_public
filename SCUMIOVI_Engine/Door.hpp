#pragma once

#include "GameObject.hpp"
#include "SceneTransitionObject.hpp"
#include <memory>

class Door : public GameObject {
private:
	SceneTransitionObject sceneTransition;
public:
	enum DoorState {
		OPEN, CLOSED
	};

	Door(std::string name,
		Vector2<> position,
		std::shared_ptr<Texture> closedTexture,
		std::shared_ptr<Texture> openTexture,
		std::shared_ptr<Scene> scene);

	// Where the player stands after walking through this door (set from the
	// "entry_pos" column of Objects.csv). Without one they keep the position
	// they had in the previous scene.
	void setEntryPosition(Vector2<> position);
	const std::optional<Vector2<>>& getEntryPosition() const;
};