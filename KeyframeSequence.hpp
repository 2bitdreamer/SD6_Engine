#pragma once

#include "Engine/Utilities/EngineCommon.hpp"

enum WrapBehavior {
	B_LOOP, B_CLAMP
};

template <typename T>
class KeyFrame
{
public:
	T m_valueAtParameter;
	float m_parameter;

public:
	KeyFrame() {

	}
};

template <typename T>
class KeyframeSequence
{

public:
	std::vector<KeyFrame<T>> m_keyFrames;
	WrapBehavior m_wrapBehavior;

public:
	T GetValueAtParameter(float parameter) const {
		//T defaultT;

		size_t numKeyFrames = m_keyFrames.size();
		FATAL_ASSERT(numKeyFrames >= 1);

		if (numKeyFrames == 1)
			return m_keyFrames[0].m_valueAtParameter;

		const KeyFrame<T>& firstKF = m_keyFrames[0];
		FATAL_ASSERT(firstKF.m_parameter == 0.f);

		const KeyFrame<T>& lastKF = m_keyFrames.back();
		float lastParam = lastKF.m_parameter;
		const T& lastValue = lastKF.m_valueAtParameter;

		if (m_wrapBehavior == B_CLAMP) {
			if (parameter > lastParam)
				return lastValue;

			parameter = Clamp(parameter, 0.f, lastParam);
		}
		else if (m_wrapBehavior == B_LOOP) {
			while (parameter > 1.f) {
				parameter -= 1.f;
			}

			while (parameter < 0.f) {
				parameter += 1.f;
			}


			if (parameter > lastParam)
				return lastValue;

// 			if (parameter > lastParam)
// 				parameter -= lastParam;
// 
// 			if (parameter < (-1.f * lastParam))
// 				parameter += lastParam;
		}

		for (size_t index = 0; index < m_keyFrames.size() - 1; index++) {
			KeyFrame<T> ks = m_keyFrames[index];
			KeyFrame<T> oneLater = m_keyFrames[index + 1];
			float fractionThisKeyframe = ks.m_parameter / lastParam;
			float fractionNextKeyframe = oneLater.m_parameter / lastParam;
			float timeSinceStart = lastParam * parameter;

			if (parameter >= fractionThisKeyframe && (parameter <= fractionNextKeyframe)) {
				float fraction = (timeSinceStart - ks.m_parameter) / (oneLater.m_parameter - ks.m_parameter);
				T interp = Interpolate(ks.m_valueAtParameter, oneLater.m_valueAtParameter, fraction);
				return interp;
			}
		}

		FATAL_ERROR("INVALID USE OF KEYFRAMESEQUENCE");
		//return defaultT;
	}

	void AddValueAtParameter(const T& value, float param) {

		if (m_keyFrames.size() <= 0) {
			KeyFrame<T> newKF;
			newKF.m_parameter = param;
			newKF.m_valueAtParameter = value;
			m_keyFrames.push_back(newKF);
			return;
		}

		KeyFrame<T> lastKF = m_keyFrames[m_keyFrames.size() - 1];
		KeyFrame<T> firstKF = m_keyFrames[0];

		if (param < firstKF.m_parameter) {
			KeyFrame<T> newKF;
			newKF.m_parameter = param;
			newKF.m_valueAtParameter = value;
			m_keyFrames.insert(m_keyFrames.begin(), newKF);
			return;
		}
		else if (param > lastKF.m_parameter) {
			KeyFrame<T> newKF;
			newKF.m_parameter = param;
			newKF.m_valueAtParameter = value;
			m_keyFrames.push_back(newKF);
			return;
		}

		for (size_t index = 0; index < m_keyFrames.size() - 1; index++) {
			KeyFrame<T> ks = m_keyFrames[index];
			KeyFrame<T> oneLater = m_keyFrames[index + 1];

			FATAL_ASSERT(param != ks.m_parameter);
			FATAL_ASSERT(param != oneLater.m_parameter);

			if (param >= ks.m_parameter && param <= oneLater.m_parameter) {
				KeyFrame<T> newKF;
				newKF.m_parameter = param;
				newKF.m_valueAtParameter = value;
				m_keyFrames.insert(m_keyFrames.begin() + index, newKF);
				return;
			}
		}
	}

	KeyframeSequence(const std::vector<KeyFrame<T>>& keyframes, WrapBehavior wb) :
		m_keyFrames(keyframes),
		m_wrapBehavior(wb)
	{

	}

	KeyframeSequence()
	{

	}
};

