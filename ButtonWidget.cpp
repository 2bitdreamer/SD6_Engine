#include "ButtonWidget.hpp"
#include "Utilities\XMLFontRenderer.hpp"



ButtonWidget::ButtonWidget() :
	m_fontRenderer(new XMLFontRenderer())
{
	SetPropertyForState("button text", UI_STATE_ALL, "Default");
	SetPropertyForState("text scale", UI_STATE_ALL, 1.f);
	SetPropertyForState("text color", UI_STATE_ALL, RGBA(255, 255, 255, 255));
	SetPropertyForState("text opacity", UI_STATE_ALL, 1.f);
}


ButtonWidget::~ButtonWidget()
{
}

void ButtonWidget::OnMouseEvent(MouseEvent me)
{

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
	GetPropertyForCurrentState("button text", textToDraw); //SHOULD NOT BE ANIMATED, or modify interpolate to be compatible

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
