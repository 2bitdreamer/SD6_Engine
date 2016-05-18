#include "EventSystem.hpp"
#include "NamedProperties.hpp"
#include <algorithm>
#include "Utilities\DevConsole.hpp"

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

int EventSystem::FireEvent(const std::string& eventName, NamedProperties& args, bool isFromConsole/*=false*/)
{

	int totalFound = 0;

	auto found = m_eventSubscriptionsStandalone.find(eventName);
	if (found != m_eventSubscriptionsStandalone.end()) {
		SubscriptionList subscribers = found->second;
		for (CallInfo callback : subscribers.m_subscriberCallbacks) {
			if (callback.m_useConsole || !isFromConsole) {
				callback.m_callback(args);
			}
		}
	}

	totalFound += m_eventSubscriptionsStandalone.size();

	auto found2 = m_eventSubscriptionObj.find(eventName);
	if (found2 != m_eventSubscriptionObj.end()) {
		ObjSubscriptionList subscriptionsForEvent = found2->second;
		for (SubscriptionBase* subscription : subscriptionsForEvent.m_subscriberCallbacks) {
			if (subscription->m_useConsole || !isFromConsole) {
				subscription->ExecuteCallback(args);
			}
		}
		totalFound += subscriptionsForEvent.m_subscriberCallbacks.size();
	}

	return totalFound;
}

void EventSystem::RegisterEventCallback(const std::string& eventName, EventCallback* callbackFunc, bool console/*=true*/)
{
	//DevConsole::GetInstance()->RegisterFunction(eventName, callbackFunc);

	CallInfo ci;
	ci.m_callback = callbackFunc;
	ci.m_useConsole = console;

	SubscriptionList& subList = m_eventSubscriptionsStandalone[eventName];
	subList.m_subscriberCallbacks.push_back(ci);
}

void EventSystem::UnregisterEvent(const std::string& eventName, EventCallback* callbackFunc) {
	SubscriptionList& subList = m_eventSubscriptionsStandalone.find(eventName)->second;

	std::vector<CallInfo>& subscriberCallbacks = subList.m_subscriberCallbacks;

	for (auto it = subscriberCallbacks.begin(); it != subscriberCallbacks.end(); ) {
		CallInfo thisInfo = *it;
		if (thisInfo.m_callback == callbackFunc) {
			it = subscriberCallbacks.erase(it);
		}
		else {
			++it;
		}
	}
}