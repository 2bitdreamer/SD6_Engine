#pragma once
#include "Assert.hpp"
#include <vector>
#include "Utilities\EngineCommon.hpp"
#include <map>

struct SubscriberList {
	std::vector<EventCallback*> m_subscriberCallbacks;
};

class EventSystem
{
public:
	EventSystem();
	~EventSystem();

	static void CreateInstance();
	static EventSystem& GetInstance();
	static void DestroyInstance();
	int FireEvent(const std::string& eventName, NamedProperties& args);
	void RegisterEvent(const std::string& eventName, EventCallback* callbackFunc);
	void UnregisterEvent(const std::string& eventName, EventCallback* callbackFunc);

private:
	static EventSystem* s_theEventSystem;
	std::map<std::string, SubscriberList> m_eventSubscriptions;

};

