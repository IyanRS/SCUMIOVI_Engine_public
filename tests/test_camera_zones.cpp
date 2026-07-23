#include "catch.hpp"
#include "Camera.hpp"
#include "CameraZone.hpp"
#include "Player.hpp"

#include <vector>

// ---------------------------------------------------------------------------
// Camera zone transitions.
//
// The strict contract under test:
//  - the camera only ever sits at a zone's framing, or moves linearly (at
//    constant velocity) between two framings during a crossing
//  - a crossing triggers exactly when the player's center passes a boundary
//  - during a crossing the player's input is locked and they are force-walked
//    to a fixed depth inside the new zone, so the crossing always ends with
//    the player framed by the new camera position and can never re-trigger
// ---------------------------------------------------------------------------

namespace {

const std::vector<CameraZone> twoZones = {
    { 0.0f, 320.0f, Vector2<>(0.0f, 0.0f) },
    { 320.0f, 640.0f, Vector2<>(320.0f, 0.0f) },
};

// Advances camera and player together, like Scene::update does.
void step(Camera& camera, std::shared_ptr<Player>& player, float seconds, float dt) {
    for (float t = 0.0f; t < seconds; t += dt) {
        camera.update(dt);
        player->update(dt);
    }
}

} // namespace

TEST_CASE("Camera zones: anchors to the player's zone on activation", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(100, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);

    camera.update(0.016f);
    REQUIRE(camera.getPosition().x == 0.0f);
    REQUIRE(camera.getPosition().y == 0.0f);

    // Anchoring is a snap, not a pan — and it doesn't touch the player.
    REQUIRE(player->canMove());
    REQUIRE(player->getPosition().x == 100.0f);
}

TEST_CASE("Camera zones: crossing right locks input, pans linearly and walks the player in", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(100, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f); // anchor in zone 0

    // Center (321) just crossed the boundary at 320.
    player->setPosition(Vector2<>(316, 50));

    const float dt = 0.1f;
    camera.update(dt); // triggers the crossing (and advances the pan by dt)
    REQUIRE_FALSE(player->canMove());

    // Pan is 320px at 320px/s: every update advances exactly 32px — constant
    // velocity, no easing, no slowdown.
    float prev = camera.getPosition().x;
    REQUIRE(prev == Approx(32.0f).margin(0.01f));
    for (int i = 0; i < 5; i++) {
        camera.update(dt);
        player->update(dt);
        float now = camera.getPosition().x;
        REQUIRE(now - prev == Approx(32.0f).margin(0.01f));
        prev = now;
    }

    // Finish the crossing (1.0s pan total).
    step(camera, player, 0.6f, dt);
    REQUIRE(camera.getPosition().x == 320.0f);
    REQUIRE(player->canMove());

    // The forced walk-in put the player's center 40px inside the new zone
    // (center 360 -> position 355), fully framed by the new camera position.
    REQUIRE(player->getPosition().x == Approx(355.0f).margin(0.01f));
    REQUIRE(player->getPosition().y == 50.0f);
}

TEST_CASE("Camera zones: crossing left is symmetric", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(400, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f); // anchor in zone 1
    REQUIRE(camera.getPosition().x == 320.0f);

    // Center (319) just crossed back below the boundary at 320.
    player->setPosition(Vector2<>(314, 50));

    step(camera, player, 1.3f, 0.05f);
    REQUIRE(camera.getPosition().x == 0.0f);
    REQUIRE(player->canMove());
    // Center 40px inside zone 0's right edge (center 280 -> position 275).
    REQUIRE(player->getPosition().x == Approx(275.0f).margin(0.01f));
}

TEST_CASE("Camera zones: a finished crossing never re-triggers or pans back", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(100, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f);

    player->setPosition(Vector2<>(316, 50));
    step(camera, player, 1.3f, 0.05f);
    REQUIRE(camera.getPosition().x == 320.0f);

    // Idle for a while: the camera must stay put and the player must not be
    // moved again — no ping-pong against the boundary that was just crossed.
    float settledX = player->getPosition().x;
    step(camera, player, 2.0f, 0.05f);
    REQUIRE(camera.getPosition().x == 320.0f);
    REQUIRE(player->getPosition().x == settledX);
    REQUIRE(player->canMove());
}

TEST_CASE("Camera zones: walking toward an edge starts the crossing early", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(255, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f); // anchor in zone 0

    // A click near (but short of) the boundary: the walk enters the 30px
    // assist band moving rightward, which must hand over to the crossing.
    player->moveToAtSpeed(Vector2<>(305, 50), 100.0f);
    step(camera, player, 2.0f, 0.05f);

    REQUIRE(camera.getPosition().x == 320.0f);
    REQUIRE(player->canMove());
    // The crossing takes over from the click walk: same fixed landing point.
    REQUIRE(player->getPosition().x == Approx(355.0f).margin(0.01f));
}

TEST_CASE("Camera zones: standing near an edge never triggers a crossing", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(295, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);

    // Center 300 is well inside the assist band, but the player is idle.
    step(camera, player, 1.0f, 0.05f);
    REQUIRE(camera.getPosition().x == 0.0f);
    REQUIRE(player->canMove());
    REQUIRE(player->getPosition().x == 295.0f);
}

TEST_CASE("Camera zones: moving away from an edge does not trigger a crossing", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(295, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f);

    // In the right-edge band but walking left, back into the zone.
    player->moveToAtSpeed(Vector2<>(100, 50), 100.0f);
    step(camera, player, 2.5f, 0.05f);

    REQUIRE(camera.getPosition().x == 0.0f);
    REQUIRE(player->canMove());
    REQUIRE(player->getPosition().x == Approx(100.0f).margin(0.01f));
}

TEST_CASE("Camera zones: teleport to a non-adjacent zone snaps instantly", "[camera][zones]") {
    const std::vector<CameraZone> threeZones = {
        { 0.0f, 320.0f, Vector2<>(0.0f, 0.0f) },
        { 320.0f, 640.0f, Vector2<>(320.0f, 0.0f) },
        { 640.0f, 960.0f, Vector2<>(640.0f, 0.0f) },
    };

    auto player = std::make_shared<Player>("zone_player", Vector2<>(100, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(threeZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f);
    REQUIRE(camera.getPosition().x == 0.0f);

    // A script drops the player two zones away: cut, no walking animation.
    player->setPosition(Vector2<>(800, 50));
    camera.update(0.016f);
    REQUIRE(camera.getPosition().x == 640.0f);
    REQUIRE(player->canMove());
    REQUIRE(player->getPosition().x == 800.0f);
}

TEST_CASE("Camera zones: setZones mid-crossing releases the player's input lock", "[camera][zones]") {
    auto player = std::make_shared<Player>("zone_player", Vector2<>(100, 50), Vector2<>(10, 10));
    Camera camera;
    camera.setZones(twoZones);
    camera.setObjectToFollow(player);
    camera.update(0.016f);

    player->setPosition(Vector2<>(316, 50));
    camera.update(0.05f); // crossing in flight
    REQUIRE_FALSE(player->canMove());

    // Scene change replaces the zones: the crossing must not keep the
    // player frozen.
    camera.clearZones();
    REQUIRE(player->canMove());
}
