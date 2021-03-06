#pragma once
#include "KeyframeSequence.hpp"
#include "Utilities\Time.hpp"
#include "KeyFrameAnimationBase.hpp"

template <typename T>
class KeyFrameAnimation : public KeyFrameAnimationBase
{
public:
	// #Eiserloh: I really really want to just see a clean, compact list of the class's
	// #Eiserloh:	capabilities.  However, since you keep putting lots of code INSIDE the
	// #Eiserloh:	class definition like this, I have to scroll down to try to get an idea
	// #Eiserloh:	of capabilities and usage.
	//
	// #Eiserloh: Other than perhaps trivial same-liners e.g. float GetQ() const { return m_q; }
	// #Eiserloh:	you should put implementations AFTER the class in .HPP, or in .impl file, or in .CPP.
	KeyFrameAnimation(const KeyframeSequence<T>& keyFrames, float duration)  :
		m_durationSeconds(duration),
		m_currentSeconds(0.f),
		m_keyFrames(keyFrames)
	{

	}

	KeyFrameAnimation() :
		m_durationSeconds(0.f),
		m_currentSeconds(0.f)
	{
		m_keyFrames.m_wrapBehavior = B_CLAMP;
	}

	~KeyFrameAnimation() {

	}

	void Update(float deltaTimeSeconds) {
		m_currentSeconds += deltaTimeSeconds;

		if (m_currentSeconds > m_durationSeconds) {
			if (m_keyFrames.m_wrapBehavior == B_LOOP)
				m_currentSeconds = 0.f;
			else if (m_keyFrames.m_wrapBehavior == B_CLAMP)
				m_currentSeconds = m_durationSeconds;
		}
			
	}

	T GetCurrentValue() const {
		if (m_durationSeconds == 0.f)
			return m_keyFrames.GetValueAtParameter(0.f);

		return m_keyFrames.GetValueAtParameter(m_currentSeconds / m_durationSeconds);
	}

	float GetCurrentSeconds() const {
		return m_currentSeconds;
	}

	void SetDuration(float seconds) {
		m_durationSeconds = seconds;
	}

	void AddAnimationFrameAtParameter(const T& value, float param) {
		m_keyFrames.AddValueAtParameter(value, param);
	}

	KeyframeSequence<T>& GetKeyframes() {
		return m_keyFrames;
	}

	void SetWrapMode(WrapBehavior wb) {
		m_keyFrames.m_wrapBehavior = wb;
	}

	virtual bool IsLooping() {
		return m_keyFrames.m_wrapBehavior == B_LOOP;
	}

	virtual void SetAnimationTime(float time) {
		m_currentSeconds = time;
	}

	virtual float GetAnimationTime() {
		return m_currentSeconds;
	}

	virtual float GetDuration() {
		return m_durationSeconds;
	}

private:
	float m_currentSeconds;
	float m_durationSeconds;
	KeyframeSequence<T> m_keyFrames;
};

