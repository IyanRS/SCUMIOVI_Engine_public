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
	Event* defaultEvent;

	VerbSyntax verbSyntax;
public:
	Verb();
	Verb(std::string name);
	Verb(std::string name, std::string displayName);
	Verb(std::string name, std::string displayName, VerbSyntax syntax);
	std::string& getDisplayName();
	std::string& getName();
	/// <summary>
	/// Sets the default event that will always be executed before the object interaction when using the verb
	/// </summary>
	/// <param name="event"></param>
	void setDefaultEvent(Event* event);
	Event* getDefaultEvent();

	std::string getInfo(std::vector<GameObject*> gameObjects);

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