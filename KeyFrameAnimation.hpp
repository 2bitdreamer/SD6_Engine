#pragma once
#include "KeyframeSequence.hpp"
#include "Utilities\Time.hpp"

template <typename T>
class KeyFrameAnimation
{
public:
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
		return m_keyFrames.GetValueAtParameter(m_currentSeconds / m_durationSeconds);
	}

	float GetCurrentSeconds() const {
		return m_currentSeconds;
	}

	void AddAnimationFrameAtParameter(const T& value, float param) {
		m_keyFrames.AddValueAtParameter(value, param);
	}

private:
	float m_currentSeconds;
	float m_durationSeconds;
	KeyframeSequence<T> m_keyFrames;
};

