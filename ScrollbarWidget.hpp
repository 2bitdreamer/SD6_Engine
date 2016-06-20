#pragma once
#include "SliderWidget.hpp"
#include "NamedProperties.hpp"
class TiXmlNode;
class ButtonWidget;

class ScrollbarWidget :
	public SliderWidget
{
public:
	ScrollbarWidget();
	std::string GetNextDecrementID();
	std::string GetNextIncrementID();
	virtual ~ScrollbarWidget();

	void IncrementEvent(NamedProperties& np);
	void DecrementEvent(NamedProperties& np);
	virtual void Render();
	virtual void Update(double deltaTimeSeconds);

	static WidgetBase* Create(const TiXmlNode* data);
	ButtonWidget* m_incButton;
	ButtonWidget* m_decButton;


	const float m_buttonSize;
	static unsigned long long s_id;
};

