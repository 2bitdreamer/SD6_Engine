#include "ProgressBarWidget.hpp"
#include "Libraries\tinyxml.h"

ProgressBarWidget::~ProgressBarWidget()
{
}

ProgressBarWidget::ProgressBarWidget() : WidgetBase()
{
	float codeDefaultDuration = 7.f;
	AddPropertyForState("inner color", UI_STATE_ALL, RGBA(50, 50, 255, 255));
	AddPropertyForState("direction", UI_STATE_ALL, C_DIRECTION_EAST);
	AddPropertyForState("duration", UI_STATE_ALL, codeDefaultDuration);

	AddPropertyForState("progress", UI_STATE_ALL, 0.f, 0.f);
	AddPropertyForState("progress", UI_STATE_ALL, 1.f, 7.f);

	SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(300.f, 350.f));
	SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(500.f, 30.f));
}


WidgetBase* ProgressBarWidget::Create(const TiXmlNode* data)
{
	ProgressBarWidget* pw = new ProgressBarWidget();
	pw->ParseAndAddEvents(data);
	pw->ParseDuration(data);
	return pw;
}

void ProgressBarWidget::Update(double deltaTimeSeconds)
{
	float deltaTimeFloat = (float)deltaTimeSeconds;
	UpdateProperty<RGBA>("inner color", deltaTimeFloat);
	UpdateProperty<CardinalDir>("direction", deltaTimeFloat);
	UpdateProperty<float>("progress", deltaTimeFloat);
	UpdateProperty<float>("duration", deltaTimeFloat);

	WidgetBase::Update(deltaTimeSeconds);
}

void ProgressBarWidget::ParseDuration(const TiXmlNode* data) {
	const char* duration = data->ToElement()->Attribute("duration");
	if (duration) {
		float durX = (float)atof(duration);
		SetStaticPropertyForState("duration", UI_STATE_ALL, durX);

		SetStaticPropertyForState("progress", UI_STATE_ALL, 0.f);
		AddPropertyForState("progress", UI_STATE_ALL, 1.f, durX);
	}
}

void ProgressBarWidget::Render()
{
	Vec2 worldPos = GetWorldPosition();
	float progress;

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float borderSize;
	GetPropertyForCurrentState("border size", borderSize);

	float opacity = GetOpacity();

	RGBA backgroundColor;
	RGBA innerColor;
	GetPropertyForCurrentState("color", backgroundColor);
	GetPropertyForCurrentState("inner color", innerColor);

	backgroundColor.a() *= opacity;
	innerColor.a() *= opacity;


	RenderBackground(worldPos, size, backgroundColor);
	GetPropertyForCurrentState("progress", progress);

	CardinalDir dir;
	GetPropertyForCurrentState("direction", dir);
	switch (dir) {
	case C_DIRECTION_EAST:
		RenderBackground(worldPos, Vec2(size.x() * progress, size.y()), innerColor);
		break;
	case C_DIRECTION_WEST: {
		Vec2 fillSize = Vec2(size.x() * progress, size.y());
		Vec2 leftEnd = Vec2(worldPos.x() + size.x() - fillSize.x(), worldPos.y());
		RenderBackground(leftEnd, fillSize, innerColor);
		break;
	}
	case C_DIRECTION_SOUTH: {
		Vec2 fillSize = Vec2(size.x(), size.y()  * progress);
		Vec2 top = Vec2(worldPos.x(), worldPos.y() + size.y() - fillSize.y());
		RenderBackground(top, fillSize, innerColor);
		break;
	}
	case C_DIRECTION_NORTH:
		RenderBackground(worldPos, Vec2(size.x(), size.y()  * progress), innerColor);
		break;
	}

	RenderOutline(worldPos, size, borderSize);

	WidgetBase::ProcessRenderEvent();
	/*
	RenderBackground(worldPos + (size * 0.25f), size * 0.5f, innerColor);
	RenderOutline(worldPos, size, borderSize);
	*/
}

