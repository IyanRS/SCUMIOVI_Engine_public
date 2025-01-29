#pragma once

#include <string>
#include "Event.hpp"

class Verb {
	std::string name;
	std::string displayName;
	Event* defaultEvent;
public:
	Verb();
	Verb(std::string name);
	Verb(std::string name, std::string displayName);
	std::string& getDisplayName();
	std::string& getName();
	/// <summary>
	/// Sets the default event that will always be executed before the object interaction when using the verb
	/// </summary>
	/// <param name="event"></param>
	void setDefaultEvent(Event* event);
	Event* getDefaultEvent();

	static Verb defaultVerb;
	static Verb anyVerb;

	static Verb walkVerb;
	static Verb pushVerb;
	static Verb closeVerb;
	static Verb openVerb;

	bool operator==(const Verb& v2) const { return name == v2.name; };
};