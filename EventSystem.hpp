#pragma once
#include "Assert.hpp"
#include <vector>
#include "Utilities\EngineCommon.hpp"
#include <map>

struct CallInfo {
	bool m_useConsole;
	EventCallback* m_callback;
};

struct SubscriptionBase {
	//virtual bool IsOfType(const type_info& compareType) = 0;
	virtual void ExecuteCallback(NamedProperties& args) = 0;
	virtual ~SubscriptionBase() { }
	bool m_useConsole;
};

template<typename T_ObjectType>
struct TypedObjectSubscription : public SubscriptionBase {
	typedef void(T_ObjectType::*MethodCallbackPtr)(NamedProperties&);

	MethodCallbackPtr m_subscriptionCallback;
	T_ObjectType& m_subscriber;

	TypedObjectSubscription(T_ObjectType& obj, MethodCallbackPtr method) :
		m_subscriber(obj),
		m_subscriptionCallback(method)
	{

	}

	virtual void ExecuteCallback(NamedProperties& args) override;
	virtual ~TypedObjectSubscription();

// 	virtual bool IsOfType(const type_info& compareType) {
// 		const type_info& myType = typeid(m_objectInstance);
// 		return (myType == compareType);
// 	}
};

template<typename T_ObjectType>
void TypedObjectSubscription<T_ObjectType>::ExecuteCallback(NamedProperties& args)
{
	(m_subscriber.*m_subscriptionCallback)(args);
}

struct SubscriptionList {
	std::vector<CallInfo> m_subscriberCallbacks;
};

struct ObjSubscriptionList {
	std::vector<SubscriptionBase*> m_subscriberCallbacks;
};




class EventSystem
{
public:
	EventSystem();
	~EventSystem();

	static void CreateInstance();
	static EventSystem& GetInstance();
	static void DestroyInstance();
	int FireEvent(const std::string& eventName, NamedProperties& args, bool isFromConsole=false);
	void RegisterEventCallback(const std::string& eventName, EventCallback* callbackFunc, bool console=true);
	void UnregisterEvent(const std::string& eventName, EventCallback* callbackFunc);
	int FireEvent(const std::string& name);

	template <typename T_OBJ_TYPE, typename T_OBJMETHOD_PTR_TYPE>
	void RegisterEventSubscription(const std::string& eventName, T_OBJ_TYPE& obj, T_OBJMETHOD_PTR_TYPE method, bool useConsole = false) {
		auto found = m_eventSubscriptionObj.find(eventName);
		ObjSubscriptionList& foundSubscription = m_eventSubscriptionObj[eventName];
		SubscriptionBase* subscription = new TypedObjectSubscription<T_OBJ_TYPE>(obj, method);
		foundSubscription.m_subscriberCallbacks.push_back(subscription);
		subscription->m_useConsole = useConsole;
	}

	template <typename T_OBJ_TYPE, typename T_OBJMETHOD_PTR_TYPE>
	void UnregisterEventSubscription(const std::string& eventName, T_OBJ_TYPE& obj, T_OBJMETHOD_PTR_TYPE method) {
		auto found = m_eventSubscriptionObj.find(eventName);

		if (found != m_eventSubscriptionObj.end()) {

			ObjSubscriptionList& foundSubscription = m_eventSubscriptionObj.find(eventName)->second;

			for (auto it = foundSubscription.m_subscriberCallbacks.begin(); it != foundSubscription.m_subscriberCallbacks.end(); ) {
				TypedObjectSubscription<T_OBJ_TYPE>* foundSubscription = (TypedObjectSubscription<T_OBJ_TYPE>*)*it;
				if ((&obj == &foundSubscription->m_subscriber) && (method == foundSubscription->m_subscriptionCallback)) {
					it = foundSubscription.m_subscriberCallbacks.erase(it);
					delete foundSubscription;
				}
				else
					++it;
			}
		}
	}

	template <typename T_OBJ_TYPE>
	void EventSystem::UnregisterEventForObject(const std::string& eventName, T_OBJ_TYPE& object)
	{
		auto found = m_eventSubscriptionObj.find(eventName);

		if (found != m_eventSubscriptionObj.end()) {

			ObjSubscriptionList& foundSubscription = m_eventSubscriptionObj.find(eventName)->second;

			for (auto subIterator = foundSubscription.m_subscriberCallbacks.begin(); subIterator != foundSubscription.m_subscriberCallbacks.end(); ) {
				TypedObjectSubscription<T_OBJ_TYPE>* foundS = (TypedObjectSubscription<T_OBJ_TYPE>*)*subIterator;
				if (&object == &foundS->m_subscriber) {
					subIterator = foundSubscription.m_subscriberCallbacks.erase(subIterator);
					delete foundS;
				}
				else
					++subIterator;
			}
		}

	}

	template <typename T_OBJ_TYPE>
	void UnregisterObjectFromAllEvents(T_OBJ_TYPE& obj, bool cleanupMemory = true) {
		for (auto it = m_eventSubscriptionObj.begin(); it != m_eventSubscriptionObj.end(); ++it) {
			ObjSubscriptionList& subList = it->second;
			for (auto it2 = subList.m_subscriberCallbacks.begin(); it2 != subList.m_subscriberCallbacks.end(); ) {
				TypedObjectSubscription<T_OBJ_TYPE>* foundSubscription = (TypedObjectSubscription<T_OBJ_TYPE>*)*it2;
				if (&foundSubscription->m_subscriber == &obj) {
					it2 = subList.m_subscriberCallbacks.erase(it2);
					if (cleanupMemory)
						delete foundSubscription;
				}
				else {
					++it2;
				}
			}
		}
	}

private:
	static EventSystem* s_theEventSystem;

	std::map<std::string, SubscriptionList> m_eventSubscriptionsStandalone;
	std::map<std::string, ObjSubscriptionList> m_eventSubscriptionObj;
};


template<typename T_ObjectType>
TypedObjectSubscription<T_ObjectType>::~TypedObjectSubscription()
{
}