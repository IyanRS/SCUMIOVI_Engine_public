#pragma once

#include "Event.hpp"
#include "json.hpp"

// -----------------------------------------------------------------------------
// resolveUIAction
//
// Resolves an action descriptor — the same shape used by a UI element's
// "onClick"/"onHover" (see UILoader) and by a key binding (see KeyBindings) —
// into an Event. Keeping this in one place means both input sources share
// exactly the same action vocabulary instead of drifting apart.
//
// Accepts either a bare string (shorthand for {"action":"script","name":<string>})
// or an object with an "action" field: script, openLayout, openOverlay,
// toggleOverlay, closeOverlay, setLocale, setVerb, save, load, quit.
// -----------------------------------------------------------------------------
Event resolveUIAction(const nlohmann::json& actionJson);
