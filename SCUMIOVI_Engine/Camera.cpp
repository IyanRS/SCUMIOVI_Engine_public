#include "Camera.hpp"

#include <SDL3/SDL.h>
#include "Common.hpp"
#include "Player.hpp"

// Camera uses a tighter tolerance than the shared EPSILON because its
// movement is measured in sub-pixel lerp increments.
constexpr float CAMERA_EPSILON = 0.00001f;

// How close to a zone's edge (in world pixels) triggers the crossing.
constexpr float ZONE_EDGE_THRESHOLD = 30.0f;
// How far past the edge the player is carried during the crossing — just
// enough to read as "walking into the next room," not a long forced walk.
constexpr float ZONE_NUDGE_DISTANCE = 24.0f;
constexpr float ZONE_NUDGE_SPEED = 100.0f;
constexpr float ZONE_TRANSITION_SECONDS = 1.0f;

Camera::Camera() {
	size = Vector2<>(RENDER_WIDTH, RENDER_HEIGHT);
}

void Camera::followObject() {
	if (auto target = objectToFollow.lock()) {
		position = target->getPosition();
	}
}

void Camera::setObjectToFollow(std::shared_ptr<GameObject> object) {
	objectToFollow = object;
}

void Camera::stopFollowingObject() {
	objectToFollow.reset();
}

Vector2<> Camera::getPosition() {
	return position;
}

Vector2<> Camera::getSize() {
	return size;
}

void Camera::setSize(Vector2<> size) {
	this->size = size;
}

void Camera::setZones(const std::vector<CameraZone>& newZones) {
	zones = newZones;
	useZones = !zones.empty();
	currentZoneIndex = -1;
}

void Camera::clearZones() {
	zones.clear();
	useZones = false;
	currentZoneIndex = -1;
}

void Camera::moveTo(Vector2<> pos, float seconds, Event callback) {
	// Already heading there — ignore duplicate calls
	if (isMoving && moveTarget.x == pos.x && moveTarget.y == pos.y)
		return;

	if (seconds <= CAMERA_EPSILON) {
		// Instantaneous
		position = pos;
		callback.execute();
		return;
	}

	isMoving     = true;
	moveStart    = position;
	moveTarget   = pos;
	moveDuration = seconds;
	moveElapsed  = 0.0f;
	moveCallback = callback;
}

// Starts a zone crossing: the player's own input is locked for its duration
// (mirrors how dialogues/cutscenes disable movement), they're carried a
// short, fixed distance past the boundary at their normal walking speed
// (replacing whatever they were doing — the crossing takes over), and the
// camera pans to the new zone's framing. Both finish together on the same
// timer, so there's a single, deterministic end to the "animation" instead
// of two independently-drifting movements that could disagree.
void Camera::beginZoneTransition(std::shared_ptr<GameObject> target, int newZoneIndex, float direction) {
	zoneTransitioning = true;
	currentZoneIndex = newZoneIndex;

	auto player = std::dynamic_pointer_cast<Player>(target);
	if (player) player->disableMovement();

	Vector2<> nudgeTarget = target->getPosition() + Vector2<>(direction * ZONE_NUDGE_DISTANCE, 0.0f);
	target->moveToAtSpeed(nudgeTarget, ZONE_NUDGE_SPEED);

	moveTo(zones[newZoneIndex].cameraTarget, ZONE_TRANSITION_SECONDS, Event([this, player]() {
		zoneTransitioning = false;
		if (player) player->enableMovement();
	}));
}

// Called once per frame by Scene::update(dt).
// Advances the camera's lerp-based movement.
void Camera::update(float deltaTime) {
	bool zoneFollowed = false;

	if (auto target = objectToFollow.lock()) {
		if (useZones) {
			float px = target->getPosition().x;

			// While a crossing is already in flight, don't re-evaluate zones
			// at all — that per-frame re-evaluation (of a still-in-progress
			// crossing) was exactly what caused both the runaway nudging and
			// the "stuck in the middle" state.
			if (!zoneTransitioning) {
				int playerZone = -1;
				for (int i = 0; i < (int)zones.size(); i++) {
					if (px >= zones[i].minX && px < zones[i].maxX) {
						playerZone = i;
						break;
					}
				}

				if (currentZoneIndex < 0) {
					// First frame with zones active: anchor immediately,
					// nothing to animate yet.
					currentZoneIndex = playerZone;
					if (playerZone >= 0) position = zones[playerZone].cameraTarget;
				}
				else if (playerZone >= 0 && playerZone != currentZoneIndex) {
					// The player ended up in a zone other than the one shown
					// without going through a normal crossing (e.g. a scene
					// reload or a script teleporting them) — just cut there,
					// there's no boundary walk to invent.
					currentZoneIndex = playerZone;
					moveTo(zones[playerZone].cameraTarget, ZONE_TRANSITION_SECONDS);
				}
				else if (playerZone == currentZoneIndex) {
					// Direction-gated so idling near an edge (or continuing
					// on beyond it) doesn't trigger a crossing back the way
					// they came; falls back to the last heading so it still
					// works the instant the player stops moving.
					Vector2<> dir = target->getDirectionVector();
					if (dir.x == 0.0f && dir.y == 0.0f) {
						dir = target->getLastMovementDirection();
					}

					bool nearRightEdge = px >= zones[playerZone].maxX - ZONE_EDGE_THRESHOLD
						&& playerZone + 1 < (int)zones.size() && dir.x > 0.0f;
					bool nearLeftEdge = px <= zones[playerZone].minX + ZONE_EDGE_THRESHOLD
						&& playerZone - 1 >= 0 && dir.x < 0.0f;

					if (nearRightEdge) {
						beginZoneTransition(target, playerZone + 1, 1.0f);
					} else if (nearLeftEdge) {
						beginZoneTransition(target, playerZone - 1, -1.0f);
					}
				}
			}

			zoneFollowed = (currentZoneIndex >= 0);
		}

		if (!zoneFollowed) {
			followObject();
			return;
		}
	}

	if (!isMoving)
		return;

	moveElapsed += deltaTime;

	if (moveElapsed >= moveDuration) {
		position = moveTarget;
		isMoving = false;
		moveCallback.execute();
	} else {
		float t = moveElapsed / moveDuration;
		position = moveStart + (moveTarget - moveStart) * t;
	}
}