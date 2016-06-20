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
	virtual void OnKeyBoardEvent(unsigned char theKey);

	static WidgetBase* Create(const TiXmlNode* data);

protected:
	int m_cursorIndex;
	std::string m_fullText;
	bool m_firstUpdate;
	int m_leftmostCharacterIndex;
	double m_currentBlinkTimeSeconds;
};

