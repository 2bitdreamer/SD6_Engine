#pragma once
#include "KeyFrameAnimation.hpp"
#include <string>

enum UIState {
	UI_STATE_DEFAULT,
	UI_STATE_HIGHLIGHTED,
	UI_STATE_PRESSED,
	UI_STATE_DISABLED,
	UI_STATE_ALL,
	NUM_UI_STATES
};

class WidgetBase
{
public:
	WidgetBase();
	~WidgetBase();

	std::string GetNameForState(UIState state);
	virtual void ApplyWidgetProperties(const NamedProperties& widgetDescriptor);
	Vec2 GetWorldPosition();
	float GetOpacity();
	template<typename T>
	PropertyGetResult GetProperty(const std::string& name, T& out_currentValue) {

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
		KeyFrameAnimation<T> asKFA;
		PropertyGetResult resultForKFA = m_stateProperties[m_currentState].Get(name, asKFA);

		if (resultForKFA == RESULT_SUCCESS)
			asKFA.Update(deltaTimeSeconds);
	}

	template<typename T>
	void SetPropertyForState(const std::string& propertyName, UIState state, T propertyValue) {
		m_stateProperties[state].Set(propertyName, propertyValue);
	}

	virtual void Update(double deltaTimeSeconds);
	void RenderOutline(const Vec2& worldPos, const Vec2& size);
	void RenderBackground(const Vec2& worldPos, const Vec2& size);
	virtual void Render();

public:
	NamedProperties m_stateProperties[ NUM_UI_STATES ];
	UIState m_currentState;
	WidgetBase* m_parentWidget;
private:
	void CopyPropertyToWidget(const std::string& propertyName, const NamedProperties& widgetDescriptor);
};

