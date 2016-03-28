#include "EventSystem.hpp"
#include "NamedProperties.hpp"
#include <algorithm>

EventSystem* EventSystem::s_theEventSystem;

EventSystem::EventSystem()
{
}


EventSystem::~EventSystem()
{
}

void EventSystem::CreateInstance()
{
	s_theEventSystem = new EventSystem();
}

EventSystem& EventSystem::GetInstance()
{
	FATAL_ASSERT(s_theEventSystem);
	return *s_theEventSystem;
}

void EventSystem::DestroyInstance()
{
	delete s_theEventSystem;
	s_theEventSystem = nullptr;
}

int EventSystem::FireEvent(const std::string& eventName, NamedProperties& args)
{
	auto found = m_eventSubscriptions.find(eventName);
	if (found == m_eventSubscriptions.end())
		return 0;

	SubscriberList subscribers = found->second;
	for (EventCallback* callback : subscribers.m_subscriberCallbacks) {
		callback(args);
	}

	return subscribers.m_subscriberCallbacks.size();
}

void EventSystem::RegisterEvent(const std::string& eventName, EventCallback* callbackFunc)
{
	SubscriberList& subList = m_eventSubscriptions[eventName];
	subList.m_subscriberCallbacks.push_back(callbackFunc);
}

void EventSystem::UnregisterEvent(const std::string& eventName, EventCallback* callbackFunc) {
	SubscriberList& subList = m_eventSubscriptions.find(eventName)->second;

	std::vector<EventCallback*>& subscriberCallbacks = subList.m_subscriberCallbacks;
	subscriberCallbacks.erase(std::remove(subscriberCallbacks.begin(), subscriberCallbacks.end(), callbackFunc), subscriberCallbacks.end());
}
