#pragma once

#include <string>
#include "Event.hpp"

#include "VerbSyntax.hpp"

#include <vector>
#include <string>

class GameObject;

class Verb {
	std::string name;
	std::string displayName;

	Event* defaultEvent;		// This event is executed just if the object of the interaction has no interaction with the used verb

	Event* beforeActualEvent;	// This event is executed before the actual event of the verb interaction
	Event* afterActualEvent;	// This event is executed after the actual event of the verb interaction

	VerbSyntax verbSyntax;

	// Newly created verbs will be added with this event as their default event
	static inline Event* defaultDefaultEvent = new Event();

	bool requiresSelectedObject;

public:
	Verb();
	Verb(std::string name);
	Verb(std::string name, std::string displayName);
	Verb(std::string name, std::string displayName, VerbSyntax syntax, bool requiresSelectedObject = false);
	std::string& getDisplayName();
	std::string& getName();
	
	void setDefaultEvent(Event* event);
	Event* getDefaultEvent();

	static void setDefaultDefaultEvent(Event* event);

	/// <summary>
	/// Sets the event that will be executed before the object interaction when using the verb
	/// </summary>
	/// <param name="event"></param>
	void setBeforeActualEvent(Event* event);
	Event* getBeforeActualEvent();

	void setAfterActualEvent(Event* event);
	Event* getAfterActualEvent();

	bool RequiresSelectedObject();

	std::string getInfo(std::vector<GameObject*> gameObjects);
	std::string getInfo(GameObject* selected, GameObject* target);

	static Verb defaultVerb;	// Represents default verb when interacting with an object
	static Verb anyVerb;		// Represents any possible verb to allow interactions that don't depend on selected verb
	static Verb selectedVerb;	// Represents the verb selected by the player

	static Verb walkVerb;
	static Verb pushVerb;
	static Verb closeVerb;
	static Verb openVerb;
	static Verb talkVerb;
	static Verb giveVerb;
	static Verb pickupVerb;
	static Verb useVerb;
	static Verb lookAtVerb;
	static Verb pullVerb;

	bool operator==(const Verb& v2) const { return name == v2.name; };
};