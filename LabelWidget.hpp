#pragma once
#include "WidgetBase.hpp"
class TiXmlNode;
class XMLFontRenderer;

class LabelWidget :
	public WidgetBase
{
public:
	LabelWidget();
	static WidgetBase* Create(const TiXmlNode* data);
	float GetTextOpacity();
	virtual void Update(double deltaTimeSeconds);
	virtual void Render();
	virtual Vec2 GetTextLowerLeft(const std::string& text, float cellSize);
	virtual ~LabelWidget();

public:
	XMLFontRenderer* m_fontRenderer;
};