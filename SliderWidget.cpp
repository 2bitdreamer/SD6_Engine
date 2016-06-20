#include "SliderWidget.hpp"
#include "Libraries\tinyxml.h"
#include <windows.h>
#include <algorithm>



SliderWidget::SliderWidget() :
	WidgetBase(),
	m_firstUpdate(true),
	m_firingRateTracker(0.0),
	m_continousFire(true)
{
	AddPropertyForState("handle color", UI_STATE_ALL, RGBA(155, 155, 155, 255));
	AddPropertyForState("handle size fraction", UI_STATE_ALL, .2f);

	AddPropertyForState("slider value", UI_STATE_ALL, 0.375f);
	AddPropertyForState("is horizontal", UI_STATE_ALL, true);
	AddPropertyForState("interval rate", UI_STATE_ALL, 0.5);

	SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(500.f, 30.f));
}


SliderWidget::~SliderWidget()
{

}

void SliderWidget::Render()
{
	float opacity = GetOpacity();

	RGBA backgroundColor;
	GetPropertyForCurrentState("color", backgroundColor);
	backgroundColor.a() *= opacity;

	RGBA handleColor;
	GetPropertyForCurrentState("handle color", handleColor);
	handleColor.a() *= opacity;

	bool isHorizontal;
	GetPropertyForCurrentState("is horizontal", isHorizontal);

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float borderSize;
	GetPropertyForCurrentState("border size", borderSize);

	RenderBackground(m_worldPosition, size, backgroundColor);

	if (isHorizontal) {
		RenderBackground(Vec2(m_buttonStartOffset, m_worldPosition.y()), Vec2(m_buttonSize, size.y()), handleColor);
	}
	else {
		RenderBackground(Vec2(m_worldPosition.x(), m_buttonStartOffset), Vec2(size.x(), m_buttonSize), handleColor);
	}

	RenderOutline(m_worldPosition, size, borderSize);
}


void SliderWidget::Update(double deltaTimeSeconds)
{
	Vec2 size;
	GetPropertyForCurrentState("size", size);

	m_worldPosition = GetWorldPosition();
	m_firingRateTracker += deltaTimeSeconds;

	bool isHorizontal;
	GetPropertyForCurrentState("is horizontal", isHorizontal);

	POINT point;
	GetCursorPos(&point);
	ScreenToClient(GetActiveWindow(), &point);
	Vec2 pos = Vec2(point.x, SCREEN_HEIGHT - point.y);


	float handleSizeFraction;
	GetPropertyForCurrentState("handle size fraction", handleSizeFraction);

	if (m_firstUpdate) {
		float value;
		GetPropertyForCurrentState("slider value", value);
		m_innerHandleClickFraction = 0.f;
		if (isHorizontal)
		{
			m_buttonSize = size.x() * handleSizeFraction;
			m_maxMovementDistance = size.x() - m_buttonSize;
			m_curHandleOffset = (value * m_maxMovementDistance) / size.x();
		}
		else
		{
			m_buttonSize = size.y() * handleSizeFraction;
			m_maxMovementDistance = size.y() - m_buttonSize;
			m_curHandleOffset = (value * m_maxMovementDistance) / size.y();
		}

		m_firstUpdate = false;
	}

	if (isHorizontal) {
		m_buttonStartOffset = m_worldPosition.x() + (size.x() * m_curHandleOffset);
		m_buttonEndOffset = m_buttonStartOffset + m_buttonSize;
	}
	else {
		m_buttonStartOffset = m_worldPosition.y() + (size.y() * m_curHandleOffset);
		m_buttonEndOffset = m_buttonStartOffset + m_buttonSize;
	}

	if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) {
		Vec2 lowerLeft;
		Vec2 lowerRight;
		Vec2 upperRight;
		Vec2 upperLeft;

		if (isHorizontal) {
			lowerLeft = Vec2(m_buttonStartOffset, m_worldPosition.y());
			lowerRight = Vec2(m_buttonStartOffset + m_buttonSize, m_worldPosition.y());
			upperRight = Vec2(m_buttonStartOffset + m_buttonSize, m_worldPosition.y() + size.y());
			upperLeft = Vec2(m_buttonStartOffset, m_worldPosition.y() + size.y());
		}
		else {
			lowerLeft = Vec2(m_worldPosition.x(), m_buttonStartOffset);
			lowerRight = Vec2(m_worldPosition.x() + size.x(), m_buttonStartOffset);
			upperRight = Vec2(m_worldPosition.x() + size.x(), m_buttonStartOffset + m_buttonSize);
			upperLeft = Vec2(m_worldPosition.x(), m_buttonStartOffset + m_buttonSize);
		}

		bool inBounds = (pos.x() >= lowerLeft.x() && pos.x() <= lowerRight.x() && pos.y() <= upperRight.y() && pos.y() >= lowerRight.y());

		float minXb = m_worldPosition.x();
		float maxXb = m_worldPosition.x() + size.x();
		float minYb = m_worldPosition.y();
		float maxYb = m_worldPosition.y() + size.y();


		bool inOuterBounds = (pos.x() >= minXb && pos.x() <= maxXb && pos.y() >= minYb && pos.y() <= maxYb);

		double intervalFireRate;
		GetPropertyForCurrentState("interval rate", intervalFireRate);

		if (!inBounds && isHorizontal && m_firingRateTracker >= intervalFireRate && inOuterBounds && m_continousFire) {
			if (pos.x() > lowerRight.x()) { // We clicked to the left of the button
				float diff = m_buttonEndOffset - m_buttonStartOffset; //how big the button is
				float maxX = m_worldPosition.x() + size.x();

				m_buttonEndOffset = fmin(m_buttonEndOffset + (m_maxMovementDistance * .1f), maxX);
				m_buttonStartOffset = m_buttonEndOffset - diff;

			}
			else {
				float diff = m_buttonEndOffset - m_buttonStartOffset;
				float minX = m_worldPosition.x() + diff;
				m_buttonEndOffset = fmax(m_buttonEndOffset - (m_maxMovementDistance * .1f), minX);
				m_buttonStartOffset = m_buttonEndOffset - diff;
			}
			m_firingRateTracker = 0.0;
		}
		else if (!inBounds && !isHorizontal && m_firingRateTracker >= intervalFireRate && inOuterBounds && m_continousFire) {
			if (pos.y() > upperRight.y()) { // Above the button
				float diff = m_buttonEndOffset - m_buttonStartOffset; //how big the button is
				float maxY = m_worldPosition.y() + size.y();

				m_buttonEndOffset = fmin(m_buttonEndOffset + (m_maxMovementDistance * .1f), maxY);
				m_buttonStartOffset = m_buttonEndOffset - diff;
			}
			else {
				float diff = m_buttonEndOffset - m_buttonStartOffset;
				float minY = m_worldPosition.y() + diff;
				m_buttonEndOffset = fmax(m_buttonEndOffset - (m_maxMovementDistance * .1f), minY);
				m_buttonStartOffset = m_buttonEndOffset - diff;
			}
			m_firingRateTracker = 0.0;
		}
		else if (inBounds) {
			m_continousFire = false;
		}
		UpdateHandleSizeFrac();
	}

	if (m_currentState == UI_STATE_PRESSED) {

		if (isHorizontal) {
			float mouseXPos = pos.x();
			m_buttonStartOffset = mouseXPos - (m_innerHandleClickFraction * m_buttonSize);
			m_buttonEndOffset = m_buttonStartOffset + m_buttonSize;

			if (m_buttonEndOffset > m_worldPosition.x() + size.x()) {
				m_buttonEndOffset = m_worldPosition.x() + size.x();
				m_buttonStartOffset = m_buttonEndOffset - m_buttonSize;
			}
			else if (m_buttonStartOffset < m_worldPosition.x()) {
				m_buttonStartOffset = m_worldPosition.x();
				m_buttonEndOffset = m_worldPosition.x() + m_buttonSize;
			}
		}
		else {
			float mouseYPos = pos.y();
			m_buttonStartOffset = mouseYPos - (m_innerHandleClickFraction * m_buttonSize);
			m_buttonEndOffset = m_buttonStartOffset + m_buttonSize;

			if (m_buttonEndOffset > m_worldPosition.y() + size.y()) {
				m_buttonEndOffset = m_worldPosition.y() + size.y();
				m_buttonStartOffset = m_buttonEndOffset - m_buttonSize;
			}
			else if (m_buttonStartOffset < m_worldPosition.y()) {
				m_buttonStartOffset = m_worldPosition.y();
				m_buttonEndOffset = m_worldPosition.y() + m_buttonSize;
			}
		}
		UpdateHandleSizeFrac();

	}

	float value;
	if (isHorizontal)
		value = (m_buttonStartOffset - m_worldPosition.x()) / m_maxMovementDistance;
	else
		value = (m_buttonStartOffset - m_worldPosition.y()) / m_maxMovementDistance;
	SetStaticPropertyForState("slider value", UI_STATE_ALL, value);

	float deltaTimeFloat = (float)deltaTimeSeconds;
	UpdateProperty<RGBA>("handle color", deltaTimeFloat);
	UpdateProperty<float>("handle size fraction", deltaTimeFloat);
	UpdateProperty<float>("slider value", deltaTimeFloat);


	DebuggerPrintf("%s %f", "VALUE!!! ", value);


	WidgetBase::Update(deltaTimeSeconds);
}

void SliderWidget::OnMouseFocusEvent(MouseEvent me)
{
	bool isHorizontal;
	GetPropertyForCurrentState("is horizontal", isHorizontal);

	if (m_currentState == UI_STATE_DISABLED)
		return;

	/*
	Check if inside the handle
	*/

	if (me.m_mouseEventType == LEFT_BUTTON_UP || me.m_mouseEventType == LEFT_BUTTON_DOWN) {
		m_continousFire = true;
	}

	Vec2 size;
	GetPropertyForCurrentState("size", size);
	m_worldPosition = GetWorldPosition();

	if (me.m_mouseEventType == LEFT_BUTTON_DOWN) {

		Vec2 lowerLeft;
		Vec2 lowerRight;
		Vec2 upperRight;
		Vec2 upperLeft;

		if (isHorizontal) {
			lowerLeft = Vec2(m_buttonStartOffset, m_worldPosition.y());
			lowerRight = Vec2(m_buttonStartOffset + m_buttonSize, m_worldPosition.y());
			upperRight = Vec2(m_buttonStartOffset + m_buttonSize, m_worldPosition.y() + size.y());
			upperLeft = Vec2(m_buttonStartOffset, m_worldPosition.y() + size.y());
		}
		else {
			lowerLeft = Vec2(m_worldPosition.x(), m_buttonStartOffset);
			lowerRight = Vec2(m_worldPosition.x() + size.x(), m_buttonStartOffset);
			upperRight = Vec2(m_worldPosition.x() + size.x(), m_buttonStartOffset + m_buttonSize);
			upperLeft = Vec2(m_worldPosition.x(), m_buttonStartOffset + m_buttonSize);
		}

		bool inBounds = (me.m_cursorPos.x() >= lowerLeft.x() && me.m_cursorPos.x() <= lowerRight.x() && me.m_cursorPos.y() <= upperRight.y() && me.m_cursorPos.y() >= lowerRight.y());

		if (m_currentState != UI_STATE_PRESSED) {
			if (isHorizontal) {
				if (inBounds) {
					m_innerHandleClickFraction = (me.m_cursorPos.x() - m_buttonStartOffset) / m_buttonSize;
					SwitchState(UI_STATE_PRESSED);
				}
			}
			else {
				if (inBounds) {
					m_innerHandleClickFraction = (me.m_cursorPos.y() - m_buttonStartOffset) / m_buttonSize;
					SwitchState(UI_STATE_PRESSED);
				}
			}
		}
	}
	else if (me.m_mouseEventType == LEFT_BUTTON_UP && m_currentState == UI_STATE_PRESSED) { //BUTTON CLICK
		std::string eventToFire;
		GetPropertyForCurrentState("click event", eventToFire);
		FireEvent(eventToFire);
		SwitchState(UI_STATE_HIGHLIGHTED);
	}
	else if (me.m_mouseEventType == MOVED && (GetKeyState(VK_LBUTTON) & 0x100) == 0) { //On the button, but not clicking
		SwitchState(UI_STATE_HIGHLIGHTED);
	}

	WidgetBase::OnMouseFocusEvent(me);
}

void SliderWidget::OnMouseUnfocusEvent(MouseEvent me)
{
	if (m_currentState == UI_STATE_DISABLED)
		return;

	Vec2 pos = me.m_cursorPos;

	Vec2 size;
	GetPropertyForCurrentState("size", size);
	m_worldPosition = GetWorldPosition();

	float minX = m_worldPosition.x();
	float maxX = m_worldPosition.x() + size.x();
	float minY = m_worldPosition.y();
	float maxY = m_worldPosition.y() + size.y();

	bool inOuterBounds = (pos.x() >= minX && pos.x() <= maxX && pos.y() >= minY && pos.y() <= maxY);

	if (me.m_mouseEventType == LEFT_BUTTON_UP) {
		m_continousFire = false;
		m_firingRateTracker = 0.0;
	}


	if (me.m_mouseEventType == LEFT_BUTTON_UP) { // Not on the button, released 
		SwitchState(UI_STATE_DEFAULT);
	}

	if (me.m_mouseEventType == MOVED && m_currentState != UI_STATE_PRESSED) { //Not on the button, moved
		SwitchState(UI_STATE_DEFAULT);
	}

	WidgetBase::OnMouseUnfocusEvent(me);
}

WidgetBase* SliderWidget::Create(const TiXmlNode* data)
{
	SliderWidget* sw = new SliderWidget();
	sw->ParseAndAddEvents(data);



	return sw;
}

void SliderWidget::UpdateHandleSizeFrac()
{
	bool isHorizontal;
	GetPropertyForCurrentState("is horizontal", isHorizontal);
	float frac;
	Vec2 size;
	GetPropertyForCurrentState("size", size);
	if (isHorizontal)
		m_curHandleOffset = (m_buttonStartOffset - m_worldPosition.x()) / size.x();
	else
		m_curHandleOffset = (m_buttonStartOffset - m_worldPosition.y()) / size.y();
}


