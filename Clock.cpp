#include "Clock.hpp"
#include "Utilities\Time.hpp"
#include <assert.h>
#include <algorithm>
#include "Utilities\EngineCommon.hpp"
#include "Assert.hpp"

float DegreesToRadians(float deg) {
	float M_PIF = (float)M_PI;
	float rad = deg*(M_PIF / 180.0f);
	return rad;
}

Clock* Clock::s_masterClock = nullptr;

/*
Clock* clock;
Vec3 pos;
clock->SetAlarm("alarm", 1.0f, pos);
*/


Clock::Clock(Clock* parentClock)
	: m_parent(parentClock),
	m_currentTime(0),
	m_deltaSeconds(0),
	m_paused(false),
	m_timeScale(1.f),
	m_maximumDelta(0.5f)
{
	FATAL_ASSERT(parentClock);
	parentClock->m_children.push_back(this);
}

Clock::Clock() :
m_currentTime(0),
m_deltaSeconds(0),
m_parent(nullptr),
m_paused(false),
m_timeScale(1.f),
m_maximumDelta(0.5f)
{

}


void Clock::InitializeMasterClock() {
	FATAL_ASSERT(s_masterClock == nullptr);
	s_masterClock = new Clock();
}

Clock::~Clock()
{
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		m_parent->m_children.push_back(*it);
	}

	std::vector<Clock*>& parentChildren = m_parent->m_children;
	parentChildren.erase(std::find(parentChildren.begin(), parentChildren.end(), this));
}


void Clock::TogglePause() {
	m_paused = !m_paused;
}


void Clock::SetAlarm(const std::string& event_name, float time_in_seconds, void* data, callback_t cb) {
	Alarm* alarm = new Alarm();
	alarm->elapsedTime = 0.f;
	alarm->name = event_name;
	alarm->onAlarmFire = cb;
	alarm->timeToFire = time_in_seconds;
	alarm->alarmFunctionData = data;
	m_alarms[event_name] = alarm;
}

void Clock::RemoveAlarm(const std::string& event_name)
{
	auto theAlarmIter = m_alarms.find(event_name);
	FATAL_ASSERT(theAlarmIter != m_alarms.end());
	delete theAlarmIter->second;
	m_alarms.erase(theAlarmIter);
}

double Clock::GetSecondsElapsed(const std::string& event_name) {
	return m_alarms[event_name]->elapsedTime;
}

void Clock::SetTimeScale(float timeScale) {
	m_timeScale = timeScale;
}

double Clock::GetSecondsRemaining(const std::string& event_name) {
	Alarm* thisAlarm = m_alarms[event_name];
	return thisAlarm->timeToFire - thisAlarm->elapsedTime;
}

double Clock::GetCurrTime() {
	return m_currentTime;
}

void Clock::AddChild(Clock* theClock) {
	m_children.push_back(theClock);
}

void Clock::AdvanceTime(float deltaSecs)
{
	deltaSecs = min(m_maximumDelta, deltaSecs);
	deltaSecs *= m_timeScale;
	if (m_paused) {
		deltaSecs = 0.f;
	}

	m_currentTime += deltaSecs;
	m_deltaSeconds = deltaSecs;

	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Clock* childClock = *it;
		childClock->AdvanceTime(deltaSecs);
	}

	for (auto it = m_alarms.begin(); it != m_alarms.end();) {
		Alarm* alarm = it->second;
		alarm->elapsedTime += deltaSecs;

		if (alarm->elapsedTime >= alarm->timeToFire) {
			it = m_alarms.erase(it);
			alarm->onAlarmFire(alarm->name, alarm->alarmFunctionData);
		}
		else {
			++it;
		}
	}
}