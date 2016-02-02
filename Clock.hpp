#pragma once
#include <vector>
#include <map>
typedef void(*callback_t)(const std::string& event_name, void* arg);

struct Alarm {
	std::string name;
	float elapsedTime;
	float timeToFire;
	callback_t onAlarmFire;
	void* alarmFunctionData;
};

class Clock
{
public:
	Clock(Clock* parent);
	static void InitializeMasterClock();
	~Clock();

	void TogglePause();
	void SetAlarm(const std::string& event_name, float time_in_seconds, void* data, callback_t cb);
	void RemoveAlarm(const std::string& event_name);

	double GetSecondsElapsed(const std::string& event_name);
	void SetTimeScale(float timeScale);
	double GetSecondsRemaining(const std::string& event_name);

	double GetCurrTime();
	void AddChild(Clock* theClock);
	void AdvanceTime(float deltaSecs);
	static Clock* s_masterClock;

	float GetDeltaSeconds() const { 
		return m_deltaSeconds; 
	}

	bool IsPaused() const { 
		return m_paused;
	}

private:
	double m_currentTime;
	float m_deltaSeconds;
	float m_timeScale;
	float m_maximumDelta;
	bool m_paused;

	std::vector<Clock*> m_children;
	std::map<std::string, Alarm*> m_alarms;
	Clock* m_parent;

private:
	Clock();
};

