#include "ScrollbarWidget.hpp"
#include "UISystem.hpp"
#include "WidgetStyle.hpp"
#include "ButtonWidget.hpp"
#include "Libraries\tinyxml.h"
#include "Utilities\EngineCommon.hpp"
#include "Engine/EventSystem.hpp"
//#include "Jen.hpp"

unsigned long long ScrollbarWidget::s_id;

ScrollbarWidget::ScrollbarWidget()
	: SliderWidget(),
	m_buttonSize(0.05f)
{
	SetStaticPropertyForState("button text color", UI_STATE_ALL, RGBA(255, 0, 0, 255));
	//SetStaticPropertyForState("is horizontal", UI_STATE_ALL, false);
	//SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(50.f, 500.f));

	WidgetStyle buttonStyle;


	m_decButton = static_cast<ButtonWidget*>(UISystem::AddStyledWidgetExplicitly("Button", "Default", buttonStyle));
	m_incButton = static_cast<ButtonWidget*>(UISystem::AddStyledWidgetExplicitly("Button", "Default", buttonStyle));

	SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(100.f, 100.f));

	m_decButton->SetStaticPropertyForState("pressed event", UI_STATE_ALL, GetNextDecrementID());
	m_incButton->SetStaticPropertyForState("pressed event", UI_STATE_ALL, GetNextIncrementID());
}

std::string ScrollbarWidget::GetNextDecrementID() {
	std::stringstream ssid;
	ssid << "ScrollbarDecrement" << s_id;
	++s_id;
	return ssid.str();
}


std::string ScrollbarWidget::GetNextIncrementID()
{
	std::stringstream ssid;
	ssid << "ScrollbarIncrement" << s_id;
	return ssid.str();
}

ScrollbarWidget::~ScrollbarWidget()
{
}


void ScrollbarWidget::IncrementEvent(NamedProperties& np)
{
	(void)np;

	bool isHorizontal;
	GetPropertyForCurrentState("is horizontal", isHorizontal);

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	if (isHorizontal) {
		float diff = m_buttonEndOffset - m_buttonStartOffset; //how big the button is
		float maxX = m_worldPosition.x() + size.x();

		m_buttonEndOffset = fmin(m_buttonEndOffset + (m_maxMovementDistance * .01f), maxX);
		m_buttonStartOffset = m_buttonEndOffset - diff;
	}
	else {
		float diff = m_buttonEndOffset - m_buttonStartOffset; //how big the button is
		float maxY = m_worldPosition.y() + size.y();

		m_buttonEndOffset = fmin(m_buttonEndOffset + (m_maxMovementDistance * .01f), maxY);
		m_buttonStartOffset = m_buttonEndOffset - diff;
	}
	UpdateHandleSizeFrac();
}

void ScrollbarWidget::DecrementEvent(NamedProperties& np)
{
	(void)np;
	bool isHorizontal;
	GetPropertyForCurrentState("is horizontal", isHorizontal);

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	if (isHorizontal) {
		float diff = m_buttonEndOffset - m_buttonStartOffset;
		float minX = m_worldPosition.x() + diff;
		m_buttonEndOffset = fmax(m_buttonEndOffset - (m_maxMovementDistance * .01f), minX);
		m_buttonStartOffset = m_buttonEndOffset - diff;
	}
	else {
		float diff = m_buttonEndOffset - m_buttonStartOffset;
		float minY = m_worldPosition.y() + diff;
		m_buttonEndOffset = fmax(m_buttonEndOffset - (m_maxMovementDistance * .01f), minY);
		m_buttonStartOffset = m_buttonEndOffset - diff;
	}
	UpdateHandleSizeFrac();
}


void ScrollbarWidget::Render()
{
	SliderWidget::Render();
}

void ScrollbarWidget::Update(double deltaTimeSeconds)
{

	if (m_firstUpdate) {
		RGBA color;
		RGBA borderColor;
		RGBA buttonTextColor;
		Vec2 size;
		float borderSize;
		bool isHorizontal;

		Vec2 worldPos;// = GetWorldPosition()
		GetPropertyForCurrentState("offset", worldPos);

		std::string incrementEvent;
		m_incButton->GetPropertyForCurrentState("pressed event", incrementEvent);

		std::string decrementEvent;
		m_decButton->GetPropertyForCurrentState("pressed event", decrementEvent);

		RegisterEventSubscriber(decrementEvent, *this, &ScrollbarWidget::DecrementEvent);
		RegisterEventSubscriber(incrementEvent, *this, &ScrollbarWidget::IncrementEvent);

		GetPropertyForCurrentState("color", color);
		GetPropertyForCurrentState("size", size);
		GetPropertyForCurrentState("border color", borderColor);
		GetPropertyForCurrentState("border size", borderSize);
		GetPropertyForCurrentState("button text color", buttonTextColor);
		GetPropertyForCurrentState("is horizontal", isHorizontal);
		float textSize;

		if (!isHorizontal)
			textSize = (size.y() * m_buttonSize) / 4.f;
		else
			textSize = size.y() * m_buttonSize * 4;

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

		if (isHorizontal) {
			m_decButton->SetStaticPropertyForState("text", UI_STATE_ALL, std::string("<"));
			m_incButton->SetStaticPropertyForState("text", UI_STATE_ALL, std::string(">"));
			m_incButton->SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(worldPos.x() + size.x(), worldPos.y()));
			m_decButton->SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(worldPos.x() - (size.x() * m_buttonSize), worldPos.y()));

			m_incButton->SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(size.x() * m_buttonSize, size.y()));
			m_decButton->SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(size.x() * m_buttonSize, size.y()));
		}
		else {
			m_decButton->SetStaticPropertyForState("text", UI_STATE_ALL, std::string("\\/"));
			m_incButton->SetStaticPropertyForState("text", UI_STATE_ALL, std::string("/\\"));
			m_incButton->SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(worldPos.x(), worldPos.y() + size.y()));
			m_decButton->SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(worldPos.x(), worldPos.y() - (size.y() * m_buttonSize)));
			m_incButton->SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(size.x(), size.y() * m_buttonSize));
			m_decButton->SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(size.x(), size.y() * m_buttonSize));
		}
	}


	SliderWidget::Update(deltaTimeSeconds);
}

WidgetBase* ScrollbarWidget::Create(const TiXmlNode* data)
{
	ScrollbarWidget* sw = new ScrollbarWidget();
	sw->ParseAndAddEvents(data);
	return sw;
}


