#pragma once
#include "LabelWidget.hpp"
#include <string>
#include <iosfwd>
class EditLineWidget :
	public LabelWidget
{
public:
	EditLineWidget();
	virtual ~EditLineWidget();

public:
	virtual void Render();
	virtual void Update(double deltaTimeSeconds);

	virtual void OnMouseFocusEvent(MouseEvent me);
	virtual void OnMouseUnfocusEvent(MouseEvent me);
	virtual void OnKeyboardEvent(unsigned char theKey);

	static WidgetBase* Create(const TiXmlNode* data);

protected:
	size_t m_cursorIndex;
	std::string m_fullText;
	bool m_firstUpdate;
	size_t m_leftmostCharacterIndex;
	size_t m_rightmostCharacterIndex;
	float m_largeCharSize;
	double m_currentBlinkTimeSeconds;
	bool m_canType;
};

