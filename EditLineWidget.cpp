#include "EditLineWidget.hpp"
#include <algorithm>
#include "LabelWidget.hpp"
#include "Utilities\XMLFontRenderer.hpp"

#undef max

EditLineWidget::EditLineWidget() 
	: LabelWidget(),
	m_cursorIndex(0),
	m_leftmostCharacterIndex(0),
	m_currentBlinkTimeSeconds(0.0)
{
	SetStaticPropertyForState("text", UI_STATE_ALL, std::string(""));
	SetStaticPropertyForState("size", UI_STATE_ALL, Vec2(500.f, 50.f));
	SetStaticPropertyForState("offset", UI_STATE_ALL, Vec2(1000.f, 500.f));


	SetStaticPropertyForState("blink time", UI_STATE_ALL, 0.5);
	/*
	AddPropertyForState("text", UI_STATE_ALL, std::string("Default"));
	AddPropertyForState("text scale", UI_STATE_ALL, 10.f);
	AddPropertyForState("text color", UI_STATE_ALL, RGBA(255, 150, 0, 255));
	AddPropertyForState("text opacity", UI_STATE_ALL, 1.f);
	*/
}


EditLineWidget::~EditLineWidget()
{

}

void EditLineWidget::Render()
{
	WidgetBase::Render();
	LabelWidget::Render();

	Vec2 worldPos = GetWorldPosition();
	float textOpacity = GetTextOpacity();

	RGBA cursorColor;
	GetPropertyForCurrentState("text color", cursorColor);
	cursorColor.a() *= textOpacity;

	float textScale;
	GetPropertyForCurrentState("text scale", textScale);

	double time;
	GetPropertyForCurrentState("blink time", time);

	if (m_currentBlinkTimeSeconds >= time) {
		Vec2 cursorSize = Vec2(1.f, textScale * 5.f);
		if (m_cursorIndex == 0) {
			RenderBackground(worldPos, cursorSize, cursorColor);
		}
		else {
			std::string leftOfCursor = m_fullText.substr(m_leftmostCharacterIndex, m_cursorIndex - m_leftmostCharacterIndex);
			float cursorOffset = m_fontRenderer->CalcTextWidth(leftOfCursor, cursorSize.y());
			RenderBackground(Vec2(worldPos.x() + cursorOffset, worldPos.y()), cursorSize, cursorColor);
		}
	}
	if (m_currentBlinkTimeSeconds >= (time * 2)) {
		m_currentBlinkTimeSeconds = 0.0;
	}
}

void EditLineWidget::Update(double deltaTimeSeconds)
{
	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float textScale;
	GetPropertyForCurrentState("text scale", textScale);


	if (m_firstUpdate) {
		
		SetStaticPropertyForState("text scale", UI_STATE_ALL, size.y() * .2f);
		m_firstUpdate = false;
	}


// 	while (m_fontRenderer->CalcTextWidth(m_fullText.substr(m_leftmostCharacterIndex, m_fullText.size()), textScale * 5.f) > size.x()) {
// 		m_leftmostCharacterIndex++;x
// 	}

	std::string rightOfCursor = m_fullText.substr(m_cursorIndex, m_fullText.size() - m_cursorIndex + 1);
	int rightmostIndex = 0;

	if (m_fontRenderer->CalcTextWidth(rightOfCursor, textScale * 5.f) > size.x()) {
		int additional = 0;
		while (((m_cursorIndex + additional) <= m_fullText.size()) && (m_fontRenderer->CalcTextWidth(m_fullText.substr(m_cursorIndex, additional + 1), textScale * 5.f) < size.x())) {
			additional++;
		}
		if (additional == 0)
			rightmostIndex = m_cursorIndex;
		else
			rightmostIndex = m_cursorIndex + additional;
	}
	else {	
		rightmostIndex = m_fullText.size() - 1;
		int additional = 0;
		while (((m_cursorIndex - additional) > 0) && (m_fontRenderer->CalcTextWidth(m_fullText.substr(m_cursorIndex - additional, rightmostIndex - (m_cursorIndex - additional) + 1), textScale * 5.f) < size.x())) {
			additional++;
		}
		if (additional == 0)
			m_leftmostCharacterIndex = m_cursorIndex;
		else
			m_leftmostCharacterIndex = m_cursorIndex - additional;
	}

	SetStaticPropertyForState("text", UI_STATE_ALL, m_fullText.substr(m_leftmostCharacterIndex, rightmostIndex - m_leftmostCharacterIndex + 1));
	LabelWidget::Update(deltaTimeSeconds);

	m_currentBlinkTimeSeconds += deltaTimeSeconds;
}

void EditLineWidget::OnMouseFocusEvent(MouseEvent me)
{
	LabelWidget::OnMouseFocusEvent(me);
}

void EditLineWidget::OnMouseUnfocusEvent(MouseEvent me)
{
	LabelWidget::OnMouseUnfocusEvent(me);
}

void EditLineWidget::OnKeyBoardEvent(unsigned char theKey)
{
	std::string aStr;
	double time;
	GetPropertyForCurrentState("blink time", time);

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float textScale;
	GetPropertyForCurrentState("text scale", textScale);

	if (theKey == VK_BACK && !m_fullText.empty()) {
		m_cursorIndex--;
		if (m_fullText.size() == 1)
			m_fullText = std::string("");
		else
			aStr = m_fullText.substr(0, m_fullText.size() - 1);
	}

	if (theKey == VK_LEFT && m_cursorIndex > 0) {
		m_cursorIndex--;
	}
	else if (theKey != VK_LEFT && theKey != VK_RIGHT) {
		std::stringstream addStr;
		if (isalnum(theKey) || ispunct(theKey)) {
			addStr << theKey;

			m_fullText.insert(m_cursorIndex, addStr.str());
			m_currentBlinkTimeSeconds = time * 1.5;
			m_cursorIndex++;
		}
	}
	else if (m_cursorIndex < m_fullText.size()) {
		if (theKey != VK_RIGHT && theKey != VK_LEFT) {
			std::stringstream addStr;
			if (isalnum(theKey) || ispunct(theKey)) {
				addStr << theKey;
				m_fullText.insert(m_cursorIndex, addStr.str());
				m_currentBlinkTimeSeconds = time * 1.5;
				m_cursorIndex++;
			}
		}
		else if (theKey == VK_RIGHT) {
			m_cursorIndex++;
		}
	}

	if (!aStr.empty())
	{
		m_fullText = aStr;
	}
}

WidgetBase* EditLineWidget::Create(const TiXmlNode* data)
{
	EditLineWidget* sw = new EditLineWidget();
	sw->ParseAndAddEvents(data);
	return sw;
}
