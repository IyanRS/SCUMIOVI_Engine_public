#pragma once

#include "Event.hpp"
#include "Vector2.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

class Camera;
class GameObject;
class Scene;
class Player;

// -----------------------------------------------------------------------------
// Cutscene
//
// A fluent builder for scripted, non-interactive sequences (camera pans,
// actor walks, dialogue, fades, scene changes) that produces an ordinary
// Event. It doesn't introduce a new execution model — it automates the same
// callback-nesting you'd otherwise hand-write (each step's completion
// callback triggers the next step), the way the dialogue codegen already
// does for branching conversations.
//
//   Cutscene()
//       .moveCamera(scene->getCamera(), pos, 2.0f)
//       .walk(hero, doorPos, 3.0f)
//       .say(hero, "Where's the map?")
//       .parallel({
//           Cutscene().walk(merchant, spot1, 1.5f),
//           Cutscene().fadeToBlack(1.0f)
//       })
//       .changeScene(nextScene, 0.5f)
//       .build();   // -> Event; execute() plays the whole sequence
//
// Steps run one after another. parallel() runs a set of sub-sequences
// concurrently and only continues once every branch has finished.
//
// Skipping: Cutscene::requestSkip() sets a flag checked between steps (not
// mid-step — a step already in progress finishes normally). A running
// cutscene that sees the flag jumps straight to its end instead of playing
// out the remaining steps, consuming the flag so it doesn't affect a later,
// unrelated cutscene. Wire it to a key binding or UI action via the
// "skipCutscene" UIAction (see UIAction.hpp) so players can skip.
// -----------------------------------------------------------------------------
class Cutscene {
public:
	// A step receives the Event to run once its own (possibly async) action
	// completes.
	using Step = std::function<void(Event next)>;

private:
	std::vector<Step> steps;
	std::shared_ptr<Player> movementDisabledFor = nullptr;

	static Event chain(const std::vector<Step>& stepsToRun, Event onDone);

public:
	Cutscene() = default;

	Cutscene& moveCamera(Camera& camera, Vector2<> position, float seconds);
	Cutscene& walk(std::shared_ptr<GameObject> object, Vector2<> position, float seconds);
	Cutscene& walkAtSpeed(std::shared_ptr<GameObject> object, Vector2<> position, float speed);
	Cutscene& say(std::shared_ptr<GameObject> object, std::string text, float seconds = 0.0f);
	Cutscene& wait(float seconds);
	Cutscene& fadeToBlack(float seconds);
	Cutscene& fadeFromBlack(float seconds);
	Cutscene& changeScene(std::shared_ptr<Scene> scene, float fadeDuration = 0.0f);

	// Runs an arbitrary action immediately (no waiting for it) before
	// continuing to the next step.
	Cutscene& run(std::function<void(EventInfo)> action);
	Cutscene& run(std::function<void()> action);

	// Runs several sub-sequences concurrently; continues once all of them finish.
	Cutscene& parallel(std::vector<Cutscene> branches);

	// Disables this player's movement input for the duration of the cutscene
	// (from the moment build()'s Event is executed) and re-enables it when
	// the cutscene ends, including if it's skipped.
	Cutscene& disableMovementFor(std::shared_ptr<Player> player);

	// Builds the final Event. Executing it plays the whole sequence.
	Event build();

	// Requests that the currently-playing cutscene (if any) skip its
	// remaining steps. Consumed by the next step boundary it reaches.
	static void requestSkip();

	// True while any built cutscene is playing. The input manager uses this
	// to hold back mouse input (walking, verbs, object clicks) until the
	// cutscene is over; key bindings stay live so skipping keeps working.
	static bool isPlaying();
};
