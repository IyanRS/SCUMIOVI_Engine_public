#pragma once

#include "GameObject.hpp"
#include "Scene.hpp"

#include <optional>

class Player;

class SceneTransitionObject : public GameObject {
private:
	std::shared_ptr<Scene> targetScene;
	// Where the player is placed on arrival. Unset means "leave them where
	// they were", which is how transitions behaved before entry positions
	// existed.
	std::optional<Vector2<>> entryPosition;
public:
	SceneTransitionObject(
		std::string name,
		Vector2<> position,
		Vector2<> size,
		std::shared_ptr<Scene> scene
	);
	std::shared_ptr<Scene> getTargetScene();

	// Declares where the player stands after coming through here (set from
	// game data — the "entry_pos" column of Objects.csv).
	void setEntryPosition(Vector2<> position);
	void clearEntryPosition();
	const std::optional<Vector2<>>& getEntryPosition() const;

	// Performs the transition: switches to the target scene and, if an entry
	// position was declared, places the player there as part of the swap.
	// Shared by this object's own walk interaction and by Door.
	void transition(float fadeDuration = 0.0f) const;
};
