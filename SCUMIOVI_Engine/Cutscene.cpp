#include "Cutscene.hpp"
#include "Game.hpp"
#include "Camera.hpp"
#include "GameObject.hpp"
#include "Player.hpp"
#include "Scene.hpp"
#include "Delay.hpp"
#include "Localization.hpp"

namespace {
	// Process-wide: cutscenes are one-at-a-time in practice (there's no
	// concept of "which cutscene" a skip request targets), so a single flag
	// consumed by whichever chain reaches a step boundary next is sufficient.
	bool skipRequested_ = false;

	bool consumeSkipRequest() {
		if (skipRequested_) {
			skipRequested_ = false;
			return true;
		}
		return false;
	}
}

void Cutscene::requestSkip() {
	skipRequested_ = true;
}

Event Cutscene::chain(const std::vector<Step>& stepsToRun, Event onDone) {
	Event next = onDone;
	for (int i = (int)stepsToRun.size() - 1; i >= 0; i--) {
		Step step = stepsToRun[i];
		Event laterSteps = next;
		next = Event(std::function<void(EventInfo)>([step, laterSteps, onDone](EventInfo e) mutable {
			if (consumeSkipRequest()) {
				onDone.execute(e);
			} else {
				step(laterSteps);
			}
		}));
	}
	return next;
}

Cutscene& Cutscene::moveCamera(Camera& camera, Vector2<> position, float seconds) {
	steps.push_back([&camera, position, seconds](Event next) {
		camera.moveTo(position, seconds, next);
	});
	return *this;
}

Cutscene& Cutscene::walk(std::shared_ptr<GameObject> object, Vector2<> position, float seconds) {
	steps.push_back([object, position, seconds](Event next) {
		object->moveTo(position, seconds, next);
	});
	return *this;
}

Cutscene& Cutscene::walkAtSpeed(std::shared_ptr<GameObject> object, Vector2<> position, float speed) {
	steps.push_back([object, position, speed](Event next) {
		object->moveToAtSpeed(position, speed, next);
	});
	return *this;
}

Cutscene& Cutscene::say(std::shared_ptr<GameObject> object, std::string text, float seconds) {
	steps.push_back([object, text, seconds](Event next) {
		object->say(tr(text), seconds, next);
	});
	return *this;
}

Cutscene& Cutscene::wait(float seconds) {
	steps.push_back([seconds](Event next) {
		Event delayed = Event(Delay(seconds));
		Event combined = delayed + next;
		combined.execute();
	});
	return *this;
}

Cutscene& Cutscene::fadeToBlack(float seconds) {
	steps.push_back([seconds](Event next) {
		Game::getInstance().getRenderer().fadeToBlack(seconds, next);
	});
	return *this;
}

Cutscene& Cutscene::fadeFromBlack(float seconds) {
	steps.push_back([seconds](Event next) {
		Game::getInstance().getRenderer().fadeFromBlack(seconds, next);
	});
	return *this;
}

Cutscene& Cutscene::changeScene(std::shared_ptr<Scene> scene, float fadeDuration) {
	steps.push_back([scene, fadeDuration](Event next) {
		Game::getInstance().changeScene(scene, fadeDuration, next);
	});
	return *this;
}

Cutscene& Cutscene::run(std::function<void(EventInfo)> action) {
	steps.push_back([action](Event next) {
		Event(action).execute();
		next.execute();
	});
	return *this;
}

Cutscene& Cutscene::run(std::function<void()> action) {
	steps.push_back([action](Event next) {
		Event(action).execute();
		next.execute();
	});
	return *this;
}

Cutscene& Cutscene::parallel(std::vector<Cutscene> branches) {
	steps.push_back([branches](Event next) {
		auto remaining = std::make_shared<int>((int)branches.size());
		if (*remaining == 0) {
			next.execute();
			return;
		}

		Event onBranchDone(std::function<void(EventInfo)>([remaining, next](EventInfo e) mutable {
			(*remaining)--;
			if (*remaining <= 0) {
				next.execute(e);
			}
		}));

		for (auto& branch : branches) {
			Event branchEvent = chain(branch.steps, onBranchDone);
			branchEvent.execute();
		}
	});
	return *this;
}

Cutscene& Cutscene::disableMovementFor(std::shared_ptr<Player> player) {
	movementDisabledFor = player;
	return *this;
}

Event Cutscene::build() {
	auto player = movementDisabledFor;

	Event onDone;
	if (player) {
		onDone = Event(std::function<void()>([player]() {
			player->enableMovement();
		}));
	}

	Event fullChain = chain(steps, onDone);

	if (player) {
		return Event(std::function<void()>([player, fullChain]() mutable {
			player->disableMovement();
			fullChain.execute();
		}));
	}
	return fullChain;
}
