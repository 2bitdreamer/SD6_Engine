#include "LabelWidget.hpp"
#include "Utilities\XMLFontRenderer.hpp"



LabelWidget::LabelWidget() : 
m_fontRenderer(new XMLFontRenderer())
{
	AddPropertyForState("text", UI_STATE_ALL, std::string("Default"));
	AddPropertyForState("text scale", UI_STATE_ALL, 10.f);
	AddPropertyForState("text color", UI_STATE_ALL, RGBA(255, 150, 0, 255));
	AddPropertyForState("text opacity", UI_STATE_ALL, 1.f);

}

WidgetBase* LabelWidget::Create(const TiXmlNode* data)
{
	LabelWidget* gw = new LabelWidget();
	gw->ParseAndAddEvents(data);
	return gw;
}

float LabelWidget::GetTextOpacity() {
	RGBA textColor;
	GetPropertyForCurrentState("text color", textColor);

	float textOpacity;
	GetPropertyForCurrentState("text opacity", textOpacity);

	float textColorAlphaAsFloat = (float)(textColor.a() / 255.f);

	return GetOpacity() * textOpacity * textColorAlphaAsFloat;
}

void LabelWidget::Update(double deltaTimeSeconds)
{
	float deltaTimeFloat = (float)deltaTimeSeconds;

	UpdateProperty<std::string>("text", deltaTimeFloat);
	UpdateProperty<float>("text scale", deltaTimeFloat);
	UpdateProperty<RGBA>("text color", deltaTimeFloat);
	UpdateProperty<float>("text opacity", deltaTimeFloat);

	WidgetBase::Update(deltaTimeSeconds);
}

/*
Completely replaces WidgetBase::Render functionality, so must check for events itself
*/
void LabelWidget::Render()
{
	std::string textToDraw;
	GetPropertyForCurrentState("text", textToDraw);

	RGBA textColor;
	GetPropertyForCurrentState("text color", textColor);

	float textScale;
	GetPropertyForCurrentState("text scale", textScale);

	float opacity;
	GetPropertyForCurrentState("opacity", opacity);

	//Vec2 size;
	//GetPropertyForCurrentState("size", size);

	float textOpacity = GetTextOpacity();

	RGBA renderColor = RGBA(textColor.r(), textColor.g(), textColor.b(), (char)(255 * textOpacity));

	float cellSize = 5.f * textScale;

	Vec3 renderPos = GetTextLowerLeft(textToDraw, cellSize);
	//renderPos.z() = -100.f;
	m_fontRenderer->DrawString(textToDraw, renderColor, renderPos, cellSize);

	ProcessRenderEvent();
}

Vec2 LabelWidget::GetTextLowerLeft(const std::string& text, float cellSize)
{
	(void)cellSize;
	(void)text;
	Vec2 worldPos = GetWorldPosition();
	return worldPos;
}




LabelWidget::~LabelWidget()
{
}
