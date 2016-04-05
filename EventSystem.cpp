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
	int totalFound = 0;

	auto found = m_eventSubscriptionsStandalone.find(eventName);
	if (found != m_eventSubscriptionsStandalone.end()) {
		SubscriptionList subscribers = found->second;
		for (EventCallback* callback : subscribers.m_subscriberCallbacks) {
			callback(args);
		}
	}

	totalFound += m_eventSubscriptionsStandalone.size();

	auto found2 = m_eventSubscriptionObj.find(eventName);
	if (found2 != m_eventSubscriptionObj.end()) {
		ObjSubscriptionList subscriptionsForEvent = found2->second;
		for (SubscriptionBase* subscription : subscriptionsForEvent.m_subscriberCallbacks) {
			subscription->ExecuteCallback(args);
		}
		totalFound += subscriptionsForEvent.m_subscriberCallbacks.size();
	}

	return totalFound;
}

void EventSystem::RegisterEventCallback(const std::string& eventName, EventCallback* callbackFunc)
{
	SubscriptionList& subList = m_eventSubscriptionsStandalone[eventName];
	subList.m_subscriberCallbacks.push_back(callbackFunc);
}

void EventSystem::UnregisterEvent(const std::string& eventName, EventCallback* callbackFunc) {
	SubscriptionList& subList = m_eventSubscriptionsStandalone.find(eventName)->second;

	std::vector<EventCallback*>& subscriberCallbacks = subList.m_subscriberCallbacks;
	subscriberCallbacks.erase(std::remove(subscriberCallbacks.begin(), subscriberCallbacks.end(), callbackFunc), subscriberCallbacks.end());
}