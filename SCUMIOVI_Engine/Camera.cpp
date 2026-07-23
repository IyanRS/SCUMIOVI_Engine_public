#include "Camera.hpp"

#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include "Common.hpp"
#include "Player.hpp"

// Camera uses a tighter tolerance than the shared EPSILON because its
// movement is measured in sub-pixel lerp increments.
constexpr float CAMERA_EPSILON = 0.00001f;

// Camera pan speed during a zone crossing, in world pixels per second.
// Speed-based (not duration-based) so every pan — whatever distance it
// covers — moves at the same constant velocity.
constexpr float ZONE_PAN_SPEED = 320.0f;
// Forced walk-in speed during a crossing; matches the normal walk speed.
constexpr float ZONE_WALK_SPEED = 100.0f;
// How far past the crossed boundary the player's center is walked — an
// absolute depth into the new zone, so wherever the trigger frame caught
// them, they always end up framed by the new camera position.
constexpr float ZONE_ENTRY_DEPTH = 40.0f;
// Near-edge assist: walking toward a boundary starts the crossing this many
// pixels early, so the player doesn't have to drag their center across the
// exact boundary pixel to change zones.
constexpr float ZONE_EDGE_MARGIN = 30.0f;
// The walk-in must land the player deeper inside the new zone than the
// assist margin reaches, or a finished crossing could immediately re-trigger
// back the way it came.
static_assert(ZONE_ENTRY_DEPTH > ZONE_EDGE_MARGIN);

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
	// A crossing must not outlive the zones it was navigating (it holds the
	// player's input lock).
	if (zoneTransitioning) endZoneTransition();
	zones = newZones;
	useZones = !zones.empty();
	currentZoneIndex = -1;
}

void Camera::clearZones() {
	if (zoneTransitioning) endZoneTransition();
	zones.clear();
	useZones = false;
	currentZoneIndex = -1;
}

void Camera::moveTo(Vector2<> pos, float seconds, Event callback) {
	// Already heading there — ignore duplicate calls
	if (isMoving && moveTarget.x == pos.x && moveTarget.y == pos.y)
		return;

	startMove(pos, seconds, callback);
}

void Camera::startMove(Vector2<> pos, float seconds, Event callback) {
	if (seconds <= CAMERA_EPSILON) {
		// Instantaneous — also cancels any move in flight, so the old lerp
		// can't drag the camera away from the snapped position next frame.
		isMoving = false;
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
// (mirrors how dialogues/cutscenes disable movement), they're walked to a
// fixed depth inside the new zone at their normal walking speed (replacing
// whatever they were doing — the crossing takes over), and the camera pans
// linearly to the new zone's framing at constant speed. The crossing ends
// when the longer of the two movements has finished, on the camera's own
// timer — so it always resolves with the player standing inside the new
// zone, framed by the new camera position.
void Camera::beginZoneTransition(std::shared_ptr<GameObject> target, int newZoneIndex, float direction) {
	zoneTransitioning = true;
	currentZoneIndex = newZoneIndex;

	auto player = std::dynamic_pointer_cast<Player>(target);
	transitionPlayer = player;
	if (player) player->disableMovement();

	const CameraZone& zone = zones[newZoneIndex];
	float width = target->getSize().x;

	// Walk-in destination: the player's center ends ZONE_ENTRY_DEPTH past
	// the boundary that was crossed. Absolute — not relative to wherever
	// the trigger frame caught the player — so the crossing always ends
	// with the player unambiguously inside the new zone and can never
	// re-trigger against the boundary it just crossed.
	float entryCenterX = direction > 0.0f
		? zone.minX + ZONE_ENTRY_DEPTH
		: zone.maxX - ZONE_ENTRY_DEPTH;
	float entryX = entryCenterX - width / 2.0f;
	// Keep the destination framed even if a zone is wider than the view.
	entryX = std::clamp(entryX, zone.cameraTarget.x, zone.cameraTarget.x + size.x - width);

	Vector2<> walkTarget(entryX, target->getPosition().y);
	float walkSeconds = std::abs(entryX - target->getPosition().x) / ZONE_WALK_SPEED;
	target->moveToAtSpeed(walkTarget, ZONE_WALK_SPEED);

	// Constant-velocity linear pan; bypasses moveTo's duplicate-target
	// guard so the pan (and the transition's timing) can never be dropped.
	float panSeconds = position.distanceTo(zone.cameraTarget) / ZONE_PAN_SPEED;
	startMove(zone.cameraTarget, panSeconds, Event());

	zoneTransitionTimeLeft = std::max(panSeconds, walkSeconds);
}

void Camera::endZoneTransition() {
	zoneTransitioning = false;
	zoneTransitionTimeLeft = 0.0f;
	if (auto player = transitionPlayer.lock()) player->enableMovement();
	transitionPlayer.reset();
}

// Called once per frame by Scene::update(dt).
// Advances the camera's lerp-based movement.
void Camera::update(float deltaTime) {
	bool zoneFollowed = false;

	if (auto target = objectToFollow.lock()) {
		if (useZones) {
			// Zones track the player's horizontal center, so a crossing
			// triggers exactly when the sprite's midpoint passes a boundary
			// — no early threshold, no direction guessing.
			float px = target->getPosition().x + target->getSize().x / 2.0f;

			// A crossing in flight owns both the camera and the player until
			// its timer runs out — zones aren't re-evaluated meanwhile, so a
			// crossing can never retrigger or reverse itself.
			if (!zoneTransitioning) {
				int playerZone = -1;
				for (int i = 0; i < (int)zones.size(); i++) {
					if (px >= zones[i].minX && px < zones[i].maxX) {
						playerZone = i;
						break;
					}
				}

				if (playerZone >= 0 && playerZone != currentZoneIndex) {
					if (currentZoneIndex >= 0 && std::abs(playerZone - currentZoneIndex) == 1) {
						// Walked across a shared boundary: constant-speed
						// linear pan plus a forced walk into the new zone.
						beginZoneTransition(target, playerZone,
							playerZone > currentZoneIndex ? 1.0f : -1.0f);
					} else {
						// First frame with zones active, or the player was
						// dropped into a non-adjacent zone (scene reload, a
						// script teleporting them): snap — there's no
						// boundary walk to animate.
						currentZoneIndex = playerZone;
						isMoving = false;
						position = zones[playerZone].cameraTarget;
					}
				}
				else if (playerZone >= 0) {
					// Near-edge assist: approaching a boundary within
					// ZONE_EDGE_MARGIN while actively walking toward it
					// starts the crossing early, so a click near the edge
					// is enough to change zones. Gated on the live movement
					// direction only — standing still near an edge never
					// triggers (the old idle-heading fallback is what made
					// crossings fire while the player wasn't going anywhere).
					Vector2<> dir = target->getDirectionVector();
					const CameraZone& zone = zones[playerZone];

					bool nearRightEdge = playerZone + 1 < (int)zones.size()
						&& px >= zone.maxX - ZONE_EDGE_MARGIN && dir.x > 0.0f;
					bool nearLeftEdge = playerZone - 1 >= 0
						&& px <= zone.minX + ZONE_EDGE_MARGIN && dir.x < 0.0f;

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

	if (isMoving) {
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

	// The crossing ends strictly on its timer, after the movement step so
	// the pan's final frame lands before input is handed back. The timer
	// covers the longer of pan and walk-in, so by the time it expires the
	// camera is at the new framing and the player is inside the new zone.
	if (zoneTransitioning) {
		zoneTransitionTimeLeft -= deltaTime;
		if (zoneTransitionTimeLeft <= 0.0f) {
			endZoneTransition();
		}
	}
}