#include "ButtonWidget.hpp"
#include "Utilities\XMLFontRenderer.hpp"
#include "UISystem.hpp"
#include "Libraries\tinyxml.h"
#include <functional>

ButtonWidget::ButtonWidget()
	: LabelWidget(),
	m_fontRenderer(new XMLFontRenderer())
{
	AddPropertyForState("text", UI_STATE_ALL, std::string("Default"));
	AddPropertyForState("text scale", UI_STATE_ALL, 10.f);
	AddPropertyForState("text color", UI_STATE_ALL, RGBA(10, 10, 10, 255));
	AddPropertyForState("text opacity", UI_STATE_ALL, 1.f);
}


ButtonWidget::~ButtonWidget()
{
}

WidgetBase* ButtonWidget::Create(const TiXmlNode* data)
{
	ButtonWidget* gw = new ButtonWidget();
	gw->ParseAndAddEvents(data);
	return gw;
}


void ButtonWidget::OnMouseFocusEvent(MouseEvent me)
{
	if (m_currentState == UI_STATE_DISABLED)
		return;

	if (me.m_mouseEventType == LEFT_BUTTON_DOWN) {
		SwitchState(UI_STATE_PRESSED);
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

void ButtonWidget::OnMouseUnfocusEvent(MouseEvent me)
{
	if (m_currentState == UI_STATE_DISABLED)
		return;

	if (me.m_mouseEventType == LEFT_BUTTON_UP) { // Not on the button, released 
		SwitchState(UI_STATE_DEFAULT);
	}

	if (me.m_mouseEventType == MOVED && m_currentState != UI_STATE_PRESSED) { //Not on the button, moved
		SwitchState(UI_STATE_DEFAULT);
	}

	WidgetBase::OnMouseUnfocusEvent(me);
}

Vec2 ButtonWidget::GetTextLowerLeft(const std::string& text, float cellSize)
{
	Vec2 size;
	GetPropertyForCurrentState("size", size);
	float textWidth = m_fontRenderer->CalcTextWidth(text, cellSize);

	Vec2 worldPos = GetWorldPosition() + (size / 2) - (Vec2(textWidth, cellSize) / 2);
	return worldPos;
}



void ButtonWidget::Render()
{
	WidgetBase::Render();
	LabelWidget::Render();
}
