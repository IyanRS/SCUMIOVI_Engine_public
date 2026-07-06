#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "Game.hpp"
#include "Verb.hpp"
#include "VerbSyntax.hpp"
#include "GameObject.hpp"
#include "EventInfo.hpp"
#include "Localization.hpp"

namespace {
	// The engine no longer creates any game-facing verbs itself (see
	// SCUMIOVI_Engine/Game.cpp) — a real game gets them from game/Verbs.csv via
	// its generated loadGame(). This test binary never runs that pipeline, so it
	// mirrors the same standard verb set here, once, before any TEST_CASE runs.
	// Keep this in sync with game/Verbs.csv if that file's verb set changes.
	void bootstrapStandardVerbsForTesting() {
		Game& game = Game::getInstance();

		game.walkVerb = std::make_shared<Verb>("walk", "Walk to");
		game.addVerb(game.walkVerb);

		game.pushVerb = std::make_shared<Verb>("push", "Push", VerbSyntax({ "Push", "Push $target" }));
		game.addVerb(game.pushVerb);
		game.pushVerb->setDefaultEvent(Event([](EventInfo e) {
			e.player.say(tr("I can't move that."));
		}));

		game.closeVerb = std::make_shared<Verb>("close", "Close", VerbSyntax({ "Close", "Close $target" }));
		game.addVerb(game.closeVerb);

		game.openVerb = std::make_shared<Verb>("open", "Open", VerbSyntax({ "Open", "Open $target" }));
		game.addVerb(game.openVerb);

		game.talkVerb = std::make_shared<Verb>("talk", "Talk to", VerbSyntax({ "Talk to", "Talk to $target" }));
		game.addVerb(game.talkVerb);

		game.giveVerb = std::make_shared<Verb>("give", "Give", VerbSyntax({ "Give", "Give $target", "Give $selected to", "Give $selected to $target" }), true);
		game.addVerb(game.giveVerb);

		game.pickupVerb = std::make_shared<Verb>("pickup", "Pick up", VerbSyntax({ "Pick up", "Pick up $target" }));
		game.addVerb(game.pickupVerb);

		game.useVerb = std::make_shared<Verb>("use", "Use", VerbSyntax({ "Use", "Use $target", "Use $selected on $target" }), true);
		game.addVerb(game.useVerb);

		game.lookAtVerb = std::make_shared<Verb>("lookat", "Look at", VerbSyntax({ "Look at", "Look at $target" }));
		game.addVerb(game.lookAtVerb);
		game.lookAtVerb->setDefaultEvent(Event([](EventInfo e) {
			auto* go = dynamic_cast<GameObject*>(&e.interactable);
			if (go && !go->getExamineText().empty()) {
				e.player.say(tr(go->getExamineText()));
			} else {
				e.player.say(tr("I don't see anything special."));
			}
		}));

		game.pullVerb = std::make_shared<Verb>("pull", "Pull", VerbSyntax({ "Pull", "Pull $target" }));
		game.addVerb(game.pullVerb);
		game.pullVerb->setDefaultEvent(Event([](EventInfo e) {
			e.player.say(tr("I can't move that."));
		}));
	}
}

int main(int argc, char* argv[]) {
	bootstrapStandardVerbsForTesting();
	return Catch::Session().run(argc, argv);
}
