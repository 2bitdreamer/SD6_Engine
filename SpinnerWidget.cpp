#include "SpinnerWidget.hpp"
#include "ButtonWidget.hpp"
#include "WidgetStyle.hpp"
#include "UISystem.hpp"
#include "Libraries\tinyxml.h"
#include "Utilities\EngineCommon.hpp"
#include "Engine/EventSystem.hpp"
#include <cctype>
unsigned long long SpinnerWidget::s_id;

SpinnerWidget::SpinnerWidget()
	: EditLineWidget(),
	m_buttonSize(0.1f),
	m_updateProgressTime(0.0),
	m_isSlowInterval(true),
	m_pressStarted(false)
{
	SetStaticPropertyForState("button text color", UI_STATE_ALL, RGBA(255, 0, 0, 255));
	SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(300.f, 50.f));

	WidgetStyle buttonStyle;


	m_decButton = static_cast<ButtonWidget*>(UISystem::AddStyledWidgetExplicitly("Button", "Default", buttonStyle));
	m_incButton = static_cast<ButtonWidget*>(UISystem::AddStyledWidgetExplicitly("Button", "Default", buttonStyle));

	SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(700.f, 100.f));

	m_decButton->SetStaticPropertyForState("pressed event", UI_STATE_ALL, GetNextPressedDecrementID());
	m_incButton->SetStaticPropertyForState("pressed event", UI_STATE_ALL, GetNextPressedIncrementID());
	m_decButton->SetStaticPropertyForState("click event", UI_STATE_ALL, GetNextClickedDecrementID());
	m_incButton->SetStaticPropertyForState("click event", UI_STATE_ALL, GetNextClickedIncrementID());

	AddPropertyForState("pressed update interval", UI_STATE_ALL, .1);
	AddPropertyForState("slow pressed update interval", UI_STATE_ALL, .5);
}

std::string SpinnerWidget::GetNextPressedDecrementID() {
	std::stringstream ssid;
	ssid << "SpinnerPressedDecrement" << s_id;
	++s_id;
	return ssid.str();
}


std::string SpinnerWidget::GetNextPressedIncrementID()
{
	std::stringstream ssid;
	ssid << "SpinnerPressedIncrement" << s_id;
	return ssid.str();
}

std::string SpinnerWidget::GetNextClickedDecrementID() {
	std::stringstream ssid;
	ssid << "SpinnerClickedDecrement" << s_id;
	++s_id;
	return ssid.str();
}


std::string SpinnerWidget::GetNextClickedIncrementID()
{
	std::stringstream ssid;
	ssid << "SpinnerClickedIncrement" << s_id;
	return ssid.str();
}


SpinnerWidget::~SpinnerWidget()
{
}

WidgetBase* SpinnerWidget::Create(const TiXmlNode* data)
{
	SpinnerWidget* sw = new SpinnerWidget();
	sw->ParseAndAddEvents(data);
	return sw;
}

int SpinnerWidget::GetInputInt()
{
	if (m_fullText == "")
		return 0;
	int inputInt;
	std::string::const_iterator it = m_fullText.begin();
	if (*it == '-')
		++it;
	bool decimalFound = false;
	bool notANumber = false;
	bool eFound = false;
	bool isScientificNotation = false;

	while (it != m_fullText.end())
	{
		if (!std::isdigit(*it))
		{
			if ((!decimalFound) && (*it == '.'))
				decimalFound = true;
			else if ((!eFound) && (*it == 'e'))
				eFound = true;
			else if (eFound && !isScientificNotation && (*it == '+'))
				isScientificNotation = true;
			else
			{
				notANumber = true;
				break;
			}
		}
		++it;
	}
	
	if (!notANumber && decimalFound)
	{
		double inputDouble = atof(m_fullText.c_str());
		inputDouble = round(inputDouble);
		inputInt = (int)inputDouble;
	}
	else if (!notANumber)
	{
		inputInt = atoi(m_fullText.c_str());
	}
	else
		inputInt = 0;
	return inputInt;
}

void SpinnerWidget::Increment()
{
	m_updateProgressTime = 0.0;
	m_isSlowInterval = false;
	int inputInt = GetInputInt();
	inputInt++;
	std::stringstream ss;
	ss << inputInt;
	std::string outputString = ss.str();
	m_fullText = outputString;
	m_cursorIndex = m_fullText.length();
	m_rightmostCharacterIndex = m_cursorIndex - 1;
	m_leftmostCharacterIndex = 0;
}

void SpinnerWidget::ClickedIncrementEvent(NamedProperties& np)
{
	(void)np;
	Increment();
	m_pressStarted = false;
}

void SpinnerWidget::PressedIncrementEvent(NamedProperties& np)
{
	(void)np;
	if (!m_pressStarted)
	{
		m_isSlowInterval = true;
		m_pressStarted = true;
		m_updateProgressTime = 0.0;
		return;
	}
	double pressInterval;
	if (m_isSlowInterval)
	{
		GetPropertyForCurrentState("slow pressed update interval", pressInterval);
	}
	else
		GetPropertyForCurrentState("pressed update interval", pressInterval);
	if (m_updateProgressTime < pressInterval)
		return;
	Increment();
}

void SpinnerWidget::Decrement()
{
	m_updateProgressTime = 0.0;
	m_isSlowInterval = false;
	int inputInt = GetInputInt();
	inputInt--;
	std::stringstream ss;
	ss << inputInt;
	std::string outputString = ss.str();
	m_fullText = outputString;
	m_cursorIndex = m_fullText.length();
	m_rightmostCharacterIndex = m_cursorIndex - 1;
	m_leftmostCharacterIndex = 0;
}

void SpinnerWidget::ClickedDecrementEvent(NamedProperties& np)
{
	(void)np;
	Decrement();
	m_pressStarted = false;
}

void SpinnerWidget::PressedDecrementEvent(NamedProperties& np)
{
	(void)np;
	if (!m_pressStarted)
	{
		m_isSlowInterval = true;
		m_pressStarted = true;
		m_updateProgressTime = 0.0;
		return;
	}
	double pressInterval;
	if (m_isSlowInterval)
	{
		GetPropertyForCurrentState("slow pressed update interval", pressInterval);
	}
	else
		GetPropertyForCurrentState("pressed update interval", pressInterval);
	if (m_updateProgressTime < pressInterval)
		return;
	Decrement();
}

void SpinnerWidget::Render()
{
	EditLineWidget::Render();
}

void SpinnerWidget::Update(double deltaTimeSeconds)
{
	RGBA color;
	RGBA borderColor;
	RGBA buttonTextColor;
	Vec2 size;
	float borderSize;

	Vec2 worldPos;// = GetWorldPosition()
	GetPropertyForCurrentState("offset", worldPos);

	if (m_firstUpdate) {

		std::string PressedIncrementEvent;
		m_incButton->GetPropertyForCurrentState("pressed event", PressedIncrementEvent);

		std::string PressedDecrementEvent;
		m_decButton->GetPropertyForCurrentState("pressed event", PressedDecrementEvent);

		std::string ClickedIncrementEvent;
		m_incButton->GetPropertyForCurrentState("click event", ClickedIncrementEvent);

		std::string ClickedDecrementEvent;
		m_decButton->GetPropertyForCurrentState("click event", ClickedDecrementEvent);

		RegisterEventSubscriber(PressedIncrementEvent, *this, &SpinnerWidget::PressedIncrementEvent);
		RegisterEventSubscriber(PressedDecrementEvent, *this, &SpinnerWidget::PressedDecrementEvent);
		RegisterEventSubscriber(ClickedIncrementEvent, *this, &SpinnerWidget::ClickedIncrementEvent);
		RegisterEventSubscriber(ClickedDecrementEvent, *this, &SpinnerWidget::ClickedDecrementEvent);

		GetPropertyForCurrentState("color", color);
		GetPropertyForCurrentState("size", size);
		GetPropertyForCurrentState("border color", borderColor);
		GetPropertyForCurrentState("border size", borderSize);
		GetPropertyForCurrentState("button text color", buttonTextColor);
		float textSize;

		textSize = (size.y() * m_buttonSize);

		m_decButton->SetStaticPropertyForState("color", UI_STATE_ALL, color);
		m_decButton->SetStaticPropertyForState("border color", UI_STATE_ALL, borderColor);
		m_decButton->SetStaticPropertyForState("text color", UI_STATE_ALL, buttonTextColor);
		m_decButton->SetStaticPropertyForState("border size", UI_STATE_ALL, borderSize);
		m_decButton->SetStaticPropertyForState("text scale", UI_STATE_ALL, textSize);

		m_incButton->SetStaticPropertyForState("color", UI_STATE_ALL, color);
		m_incButton->SetStaticPropertyForState("border color", UI_STATE_ALL, borderColor);
		m_incButton->SetStaticPropertyForState("text color", UI_STATE_ALL, buttonTextColor);
		m_incButton->SetStaticPropertyForState("border size", UI_STATE_ALL, borderSize);
		m_incButton->SetStaticPropertyForState("text scale", UI_STATE_ALL, textSize);

		m_decButton->SetStaticPropertyForState("text", UI_STATE_ALL, std::string("\\/"));
		m_incButton->SetStaticPropertyForState("text", UI_STATE_ALL, std::string("/\\"));
		m_incButton->SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(worldPos.x() + size.x(), worldPos.y() + (size.y() / 2)));
		m_decButton->SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(worldPos.x() + size.x(), worldPos.y()));

		m_incButton->SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(size.x() * m_buttonSize, size.y() / 2));
		m_decButton->SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(size.x() * m_buttonSize, size.y() / 2));
		m_firstUpdate = false;
	}
	EditLineWidget::Update(deltaTimeSeconds);
	m_updateProgressTime += deltaTimeSeconds;
}