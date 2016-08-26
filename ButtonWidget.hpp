#pragma once
#include "LabelWidget.hpp"
class XMLFontRenderer;
class TiXmlNode;

class ButtonWidget : public LabelWidget	
{
public:
	ButtonWidget();
	~ButtonWidget();

	static WidgetBase* Create(const TiXmlNode* data);
	virtual void OnMouseFocusEvent(MouseEvent me);
	virtual void OnMouseUnfocusEvent(MouseEvent me);
	virtual void Render();
	virtual Vec2 GetTextLowerLeft(const std::string& text, float cellSize);

public:
	XMLFontRenderer* m_fontRenderer;
// 	std::string m_buttonText;
// 	float m_textScale;
// 	RGBA m_textColor;
// 	float m_textOpacity;
};

