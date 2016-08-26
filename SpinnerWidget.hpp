#pragma once
#include "EditLineWidget.hpp"

#include<string>

class TiXmlNode;
class ButtonWidget;

class SpinnerWidget :
	public EditLineWidget
{
public:
	SpinnerWidget();
	virtual ~SpinnerWidget();
	static WidgetBase* Create(const TiXmlNode* data);
	virtual void Render();
	virtual void Update(double deltaTimeSecond);
	void PressedIncrementEvent(NamedProperties& np);
	void PressedDecrementEvent(NamedProperties& np);
	void ClickedIncrementEvent(NamedProperties& np);
	void ClickedDecrementEvent(NamedProperties& np);
	
	int GetInputInt();

	std::string GetNextPressedIncrementID();
	std::string GetNextPressedDecrementID();
	std::string GetNextClickedIncrementID();
	std::string GetNextClickedDecrementID();

	void Increment();
	void Decrement();

	ButtonWidget* m_incButton;
	ButtonWidget* m_decButton;

	static unsigned long long s_id;
	float m_buttonSize;
	double m_updateProgressTime;
	bool m_isSlowInterval;
	bool m_pressStarted;
};

