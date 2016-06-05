#include "ButtonWidget.hpp"
#include "Utilities\XMLFontRenderer.hpp"
#include "UISystem.hpp"
#include "Libraries\tinyxml.h"
#include <functional>

ButtonWidget::ButtonWidget() :
	m_fontRenderer(new XMLFontRenderer())
{
	SetPropertyForState("button text", UI_STATE_ALL, std::string("Default"));
	SetPropertyForState("text scale", UI_STATE_ALL, 1.f);
	SetPropertyForState("text color", UI_STATE_ALL, RGBA(255, 255, 255, 255));
	SetPropertyForState("text opacity", UI_STATE_ALL, 1.f);
}


ButtonWidget::~ButtonWidget()
{
}

WidgetBase* ButtonWidget::Create(const TiXmlNode* data)
{
	ButtonWidget* gw = new ButtonWidget();
	const char* clickEventToFire = data->ToElement()->Attribute("OnClick");
	gw->SetPropertyForState("click event", UI_STATE_PRESSED, std::string(clickEventToFire));
	return gw;
}


void ButtonWidget::OnMouseEvent(MouseEvent me)
{
	m_currentState = UI_STATE_PRESSED;
	std::string eventToFire;
	//GetPropertyForCurrentState("click event", eventToFire);
	FireEvent(eventToFire);
}

void ButtonWidget::Update(double deltaTimeSeconds)
{
	float deltaTimeFloat = (float)deltaTimeSeconds;

	UpdateProperty<std::string>("button text", deltaTimeFloat);
	UpdateProperty<float>("text scale", deltaTimeFloat);
	UpdateProperty<RGBA>("text color", deltaTimeFloat);
	UpdateProperty<float>("text opacity", deltaTimeFloat);

	WidgetBase::Update(deltaTimeSeconds);
}

float ButtonWidget::GetTextOpacity() {
	float textOpacity;
	GetPropertyForCurrentState("text opacity", textOpacity);
	return m_parentWidget->GetOpacity() * textOpacity;
}

Vec2 ButtonWidget::GetCenter(const std::string& text, float cellSize)
{	
	Vec2 size;
	GetPropertyForCurrentState("size", size);
	float textWidth = m_fontRenderer->CalcTextWidth(text, cellSize);

	Vec2 worldPos = GetWorldPosition() + (size / 2) - Vec2(textWidth / 2.f, cellSize / 2.f);
	return worldPos;
}



void ButtonWidget::Render()
{
	std::string textToDraw;
	GetPropertyForCurrentState("button text", textToDraw);

	RGBA textColor;
	GetPropertyForCurrentState("text color", textColor);

	float textScale;
	GetPropertyForCurrentState("text scale", textScale);

	float textOpacity;
	GetPropertyForCurrentState("text opacity", textOpacity);

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	RGBA renderColor = RGBA(textColor.r(), textColor.g(), textColor.b(), GetTextOpacity());

	float cellSize = size.y() * textScale;
	m_fontRenderer->DrawString(textToDraw, textColor, GetCenter(textToDraw, cellSize));

	WidgetBase::Render();
}
