#pragma once

#include <string>
#include "Event.hpp"

#include "VerbSyntax.hpp"

#include <vector>
#include <string>

#include <memory>

class GameObject;

class Verb {
	std::string name;
	std::string displayName;

	Event defaultEvent;		// This event is executed just if the object of the interaction has no interaction with the used verb
	Event beforeActualEvent;	// This event is executed before the actual event of the verb interaction
	Event afterActualEvent;	// This event is executed after the actual event of the verb interaction

	VerbSyntax verbSyntax;

	static inline Event defaultDefaultEvent = Event();

	bool requiresSelectedObject;

public:
	Verb();
	Verb(std::string name);
	Verb(std::string name, std::string displayName);
	Verb(std::string name, std::string displayName, VerbSyntax syntax, bool requiresSelectedObject = false);
	// Returns the localized display name (resolved through tr()), so it is a
	// value rather than a reference into the stored source string.
	std::string getDisplayName();
	std::string& getName();
	
	void setDefaultEvent(const Event& event);
	Event getDefaultEvent();

	static void setDefaultDefaultEvent(const Event& event);

	/// <summary>
	/// Sets the event that will be executed before the object interaction when using the verb
	/// </summary>
	/// <param name="event"></param>
	void setBeforeActualEvent(const Event& event);
	Event getBeforeActualEvent();

	void setAfterActualEvent(const Event& event);
	Event getAfterActualEvent();

	bool RequiresSelectedObject();

	std::string getInfo(std::vector<std::shared_ptr<GameObject>> gameObjects);
	std::string getInfo(std::shared_ptr<GameObject> selected, std::shared_ptr<GameObject> target);

	bool operator==(const Verb& v2) const { return name == v2.name; };
};