#pragma once
#include "WidgetBase.hpp"
class XMLFontRenderer;

class ButtonWidget :
	public WidgetBase
{
public:
	ButtonWidget();
	~ButtonWidget();

	virtual void OnMouseEvent(MouseEvent me);
	virtual void Update(double deltaTimeSeconds);
	float GetTextOpacity();
	Vec2 GetCenter(const std::string& text, float cellSize);
	virtual void Render();

public:
	XMLFontRenderer* m_fontRenderer;
// 	std::string m_buttonText;
// 	float m_textScale;
// 	RGBA m_textColor;
// 	float m_textOpacity;
};

