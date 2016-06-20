#pragma once
#include "KeyFrameAnimation.hpp"
#include <string>
class WidgetStyle;
class TiXmlNode;

class WidgetBase
{
public:
	WidgetBase();
	~WidgetBase();

	static std::string GetNextDecrementID();
	static std::string GetNameForState(UIState state);
	static UIState GetStateForName(const std::string& name);


	virtual void ApplyWidgetProperties(const NamedProperties& widgetDescriptor);
	virtual void OnMouseFocusEvent(MouseEvent me);
	virtual void OnMouseUnfocusEvent(MouseEvent me);
	virtual void OnKeyBoardEvent(unsigned char theKey);


	Vec2 GetWorldPosition();
	float GetOpacity();
	template<typename T>
	PropertyGetResult GetPropertyForCurrentState(const std::string& name, T& out_currentValue) {

		KeyFrameAnimation<T> asAnimation;
		T asStaticValue;

		PropertyGetResult resultAnim = m_stateProperties[m_currentState].Get(name, asAnimation);
		if (resultAnim == RESULT_FAILED_WRONG_TYPE) {
			PropertyGetResult asStaticValueResult = m_stateProperties[m_currentState].Get(name, asStaticValue);

			if (asStaticValueResult != RESULT_SUCCESS)
				return asStaticValueResult;
			else {
				out_currentValue = asStaticValue;
				return RESULT_SUCCESS;
			}

		}
		else if (resultAnim == RESULT_SUCCESS) {
			out_currentValue = asAnimation.GetCurrentValue();
			return RESULT_SUCCESS;
		}

		return resultAnim;
	}

	template <typename T>
	void UpdateProperty(const std::string& name, float deltaTimeSeconds) {
		// 		KeyFrameAnimation<T> asKFA;
		// 		PropertyGetResult resultForKFA = m_stateProperties[m_currentState].Get(name, asKFA);

		for (UIState st = UI_STATE_DEFAULT; st < NUM_UI_STATES; st = (UIState)(st + 1)) {
			KeyFrameAnimation<T>& asKFA = m_stateProperties[st].Get<KeyFrameAnimation<T>>(name);

			KeyframeSequence<T>& keyframes = asKFA.GetKeyframes();
			if (keyframes.m_wrapBehavior == B_LOOP)
				asKFA.Update(deltaTimeSeconds);
			else if (keyframes.m_wrapBehavior == B_CLAMP && (m_currentState == st))
				asKFA.Update(deltaTimeSeconds);
		}
	}


	template<typename T>
	void AddPropertyForState(const std::string& propertyName, UIState state, T propertyValue, float animationTime=0.f) {
		KeyFrameAnimation<T> asKFA;

		if (state == UI_STATE_ALL) {
			for (UIState st = UI_STATE_DEFAULT; st < NUM_UI_STATES; st = (UIState)(st + 1)) {
				m_stateProperties[st].Get<KeyFrameAnimation<T>>(propertyName, asKFA);
				asKFA.AddAnimationFrameAtParameter(propertyValue, animationTime);

				if (animationTime > asKFA.GetDuration()) {
					asKFA.SetDuration(animationTime);
				}

				m_stateProperties[st].Set(propertyName, asKFA);
			}
		}
		else {
			m_stateProperties[state].Get<KeyFrameAnimation<T>>(propertyName, asKFA);
			asKFA.AddAnimationFrameAtParameter(propertyValue, animationTime);

			if (animationTime > asKFA.GetDuration()) {
				asKFA.SetDuration(animationTime);
			}

			m_stateProperties[state].Set(propertyName, asKFA);
		}
	}

	template<typename T>
	void SetStaticPropertyForState(const std::string& propertyName, UIState state, T propertyValue) {
		KeyFrameAnimation<T> asKFA;

		if (state == UI_STATE_ALL) {
			for (UIState st = UI_STATE_DEFAULT; st < NUM_UI_STATES; st = (UIState)(st + 1)) {
				asKFA.AddAnimationFrameAtParameter(propertyValue, 0.f);
				m_stateProperties[st].Set(propertyName, asKFA);
			}
		}
		else {
			asKFA.AddAnimationFrameAtParameter(propertyValue, 0.f);
			m_stateProperties[state].Set(propertyName, asKFA);
		}
	}

	virtual void Update(double deltaTimeSeconds);
	void ProcessUpdateEvent();

	virtual void Render();
	void ProcessRenderEvent();

	virtual void SwitchState(UIState newState);
	virtual void ParseAndAddEvents(const TiXmlNode* data);

	void RenderOutline(const Vec2& worldPos, const Vec2& widgetSize, float lineWidth);
	void RenderBackground(const Vec2& worldPos, const Vec2& size, const RGBA& backgroundColor);
	

	static WidgetBase* Create(const TiXmlNode* data);
	void ApplyGeneralStyleToAll(WidgetStyle* baseStyle);
	void CopyStatePropertyToWidget(UIState state, const NamedProperties& currentNP);

	void ApplyStyle(WidgetStyle* baseStyle);
public:
	NamedProperties m_stateProperties[ NUM_UI_STATES ];
	UIState m_currentState;
	WidgetBase* m_parentWidget;
	static int s_currentID;
};

