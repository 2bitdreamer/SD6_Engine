#include "CheckBoxWidget.hpp"
#include "WidgetBase.hpp"



CheckBoxWidget::CheckBoxWidget() : ButtonWidget()
{
	AddPropertyForState("text opacity", UI_STATE_ALL, 0.f);
	AddPropertyForState("text", UI_STATE_ALL, std::string(""));
	AddPropertyForState("checked", UI_STATE_ALL, false);
	AddPropertyForState("inner color", UI_STATE_ALL, RGBA(0, 0, 0, 255));
	SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(50.f, 50.f));
}

void CheckBoxWidget::ToggleChecked() {
	bool val = !IsChecked();
	SetStaticPropertyForState("checked", UI_STATE_ALL, val);
}


bool CheckBoxWidget::IsChecked() {
	bool checked;
	GetPropertyForCurrentState("checked", checked);
	return checked;
}

CheckBoxWidget::~CheckBoxWidget()
{
}

WidgetBase* CheckBoxWidget::Create(const TiXmlNode* data)
{
	CheckBoxWidget* gw = new CheckBoxWidget();
	gw->ParseAndAddEvents(data);
	return gw;
}

void CheckBoxWidget::OnMouseFocusEvent(MouseEvent me)
{
	if (m_currentState == UI_STATE_DISABLED)
		return;

	if (me.m_mouseEventType == LEFT_BUTTON_DOWN) {
		SwitchState(UI_STATE_PRESSED);
	}
	else if (me.m_mouseEventType == LEFT_BUTTON_UP && m_currentState == UI_STATE_PRESSED) { //BUTTON CLICK
		ToggleChecked();
		SwitchState(UI_STATE_HIGHLIGHTED);
	}
	else if (me.m_mouseEventType == MOVED && (GetKeyState(VK_LBUTTON) & 0x100) == 0) { //On the button, but not clicking
		SwitchState(UI_STATE_HIGHLIGHTED);
	}

	WidgetBase::OnMouseFocusEvent(me);
}

void CheckBoxWidget::OnMouseUnfocusEvent(MouseEvent me)
{
	ButtonWidget::OnMouseUnfocusEvent(me);
}

void CheckBoxWidget::Render()
{ 
	Vec2 worldPos = GetWorldPosition();

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float borderSize;
	GetPropertyForCurrentState("border size", borderSize);

	float opacity = GetOpacity();

	RGBA backgroundColor;
	RGBA innerColor;
	GetPropertyForCurrentState("color", backgroundColor);
	
	backgroundColor.a() *= opacity;
	
	RenderBackground(worldPos, size, backgroundColor);

	if (IsChecked()) {
		GetPropertyForCurrentState("inner color", innerColor);
		innerColor.a() *= opacity;

		RenderBackground(worldPos + (size * 0.25f), size * 0.5f, innerColor);
	}

	RenderOutline(worldPos, size, borderSize);
}

void CheckBoxWidget::Update(double deltaTimeSeconds)
{
	float deltaTimeFloat = (float)deltaTimeSeconds;

	UpdateProperty<bool>("checked", deltaTimeFloat);
	UpdateProperty<RGBA>("inner color", deltaTimeFloat);

	WidgetBase::Update(deltaTimeSeconds);
}
