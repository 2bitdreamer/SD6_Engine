#pragma once
#include "KeyFrameAnimation.hpp"
#include <string>
class WidgetStyle;

class WidgetBase
{
public:
	WidgetBase();
	~WidgetBase();

	static std::string GetNameForState(UIState state);
	static UIState GetStateForName(const std::string& name);


	virtual void ApplyWidgetProperties(const NamedProperties& widgetDescriptor);
	virtual void OnMouseEvent(MouseEvent me);

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

	template<typename T>
	PropertyGetResult SetPropertyForCurrentState(const std::string& name, T newValue) {
		m_stateProperties[m_currentState].Get(name, newValue);
	}

	template <typename T>
	void UpdateProperty(const std::string& name, float deltaTimeSeconds) {
// 		KeyFrameAnimation<T> asKFA;
// 		PropertyGetResult resultForKFA = m_stateProperties[m_currentState].Get(name, asKFA);

		KeyFrameAnimation<T>& asKFA = m_stateProperties[m_currentState].Get<KeyFrameAnimation<T>>(name);
		asKFA.Update(deltaTimeSeconds); 		
	}

	template<typename T>
	void SetPropertyForState(const std::string& propertyName, UIState state, T propertyValue, float animationTime=0.f) {
		KeyFrameAnimation<T> asKFA;

		if (state == UI_STATE_ALL) {
			for (UIState st = UI_STATE_DEFAULT; st < NUM_UI_STATES; st = (UIState)(st + 1)) {
				m_stateProperties[st].Get<KeyFrameAnimation<T>>(propertyName, asKFA);
				asKFA.AddAnimationFrameAtParameter(propertyValue, animationTime);
				m_stateProperties[st].Set(propertyName, asKFA);
			}
		}
		else {
			m_stateProperties[state].Get<KeyFrameAnimation<T>>(propertyName, asKFA);
			asKFA.AddAnimationFrameAtParameter(propertyValue, animationTime);
			m_stateProperties[state].Set(propertyName, asKFA);
		}
	}

	virtual void Update(double deltaTimeSeconds);
	virtual void Render();

	void RenderOutline(const Vec2& worldPos, const Vec2& widgetSize, float lineWidth);
	void RenderBackground(const Vec2& worldPos, const Vec2& size);

	static WidgetBase* Create();
	void CopyStatePropertyToWidget(UIState state, const NamedProperties& currentNP);

	void ApplyStyle(WidgetStyle* baseStyle);
public:
	NamedProperties m_stateProperties[ NUM_UI_STATES ];
	UIState m_currentState;
	WidgetBase* m_parentWidget;
};

