#include "EditLineWidget.hpp"
#include <algorithm>
#include "LabelWidget.hpp"
#include "Utilities\XMLFontRenderer.hpp"
#include "SD7c\SD5\GameCode\theGame.hpp"

#undef max

EditLineWidget::EditLineWidget()
	: LabelWidget(),
	m_cursorIndex(0),
	m_leftmostCharacterIndex(0),
	m_rightmostCharacterIndex(0),
	m_currentBlinkTimeSeconds(0.0),
	m_largeCharSize(0.0f),
	m_canType(false)
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

	if ((m_currentBlinkTimeSeconds >= time) && m_canType) {
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
		m_largeCharSize = m_fontRenderer->CalcTextWidth("Q", textScale * 5.f);
		SetStaticPropertyForState("text scale", UI_STATE_ALL, size.y() * .2f);
		m_firstUpdate = false;
	}
	if (!(m_fullText.size() == 0))
	{
		std::string curText = m_fullText.substr(m_leftmostCharacterIndex,
			m_rightmostCharacterIndex - m_leftmostCharacterIndex + 1);
		SetStaticPropertyForState("text", UI_STATE_ALL, curText);
	}
	LabelWidget::Update(deltaTimeSeconds);
	
	if (m_canType)
	{
		if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) {
			POINT point;
			GetCursorPos(&point);
			ScreenToClient(GetActiveWindow(), &point);
			Vec2 mouseCoord = Vec2((float)point.x, (float)(SCREEN_HEIGHT - point.y));
			Vec2 offset;
			GetPropertyForCurrentState("offset", offset);
			Vec2 maxBounds = offset + size;
			if (!(offset.x() <= mouseCoord.x() && offset.y() <= mouseCoord.y() && mouseCoord.x() <= maxBounds.x() && mouseCoord.y() <= maxBounds.y())) {
				m_canType = false;
			}
		}
		m_currentBlinkTimeSeconds += deltaTimeSeconds;
	}

}

void EditLineWidget::OnMouseFocusEvent(MouseEvent me)
{
	if (m_currentState == UI_STATE_DISABLED)
		return;

	if (me.m_mouseEventType == LEFT_BUTTON_DOWN) {
		SwitchState(UI_STATE_PRESSED);
		m_canType = true;
	}
	else if (me.m_mouseEventType == LEFT_BUTTON_UP && m_currentState == UI_STATE_PRESSED) { //BUTTON CLICK
		std::string eventToFire;
		GetPropertyForCurrentState("click event", eventToFire);
		FireEvent(eventToFire);
		SwitchState(UI_STATE_HIGHLIGHTED);
	}
	else if (me.m_mouseEventType == MOVED && (GetKeyState(VK_LBUTTON) & 0x100) == 0) { //On the button, but not clicking
		SwitchState(UI_STATE_HIGHLIGHTED);
	}
	LabelWidget::OnMouseFocusEvent(me);
}

void EditLineWidget::OnMouseUnfocusEvent(MouseEvent me)
{
	if (m_currentState == UI_STATE_DISABLED)
		return;

	if (me.m_mouseEventType == LEFT_BUTTON_UP) { // Not on the button, released 
		SwitchState(UI_STATE_DEFAULT);
	}

	if (me.m_mouseEventType == MOVED && m_currentState != UI_STATE_PRESSED) { //Not on the button, moved
		SwitchState(UI_STATE_DEFAULT);
	}

	LabelWidget::OnMouseUnfocusEvent(me);
}

void EditLineWidget::OnKeyBoardEvent(unsigned char theKey)
{
	if (m_canType)
	{
		TheGame& game = TheGame::GetInstance();
		if ((game.m_keyStates[VK_UP].m_isPressed) || (game.m_keyStates[VK_DOWN].m_isPressed))
		{
			return;
		}
		std::string aStr;
		double time;
		GetPropertyForCurrentState("blink time", time);

		Vec2 size;
		GetPropertyForCurrentState("size", size);

		float textScale;
		GetPropertyForCurrentState("text scale", textScale);

		float textBoundary = size.x() - m_largeCharSize;
		float textScreenSize = m_fontRenderer->CalcTextWidth(m_fullText, textScale * 5.f);


		if (game.m_keyStates[VK_LEFT].m_isPressed)
		{
			if (m_cursorIndex > 0) {
				m_cursorIndex--;
				m_currentBlinkTimeSeconds = time * 1.5;
				if ((textScreenSize > textBoundary) && (m_leftmostCharacterIndex > 0)
					&& (m_cursorIndex < m_leftmostCharacterIndex))
				{
					m_leftmostCharacterIndex--;
					m_rightmostCharacterIndex--;
				}
			}
		}
		else if (game.m_keyStates[VK_RIGHT].m_isPressed)
		{
			if (m_cursorIndex < m_fullText.size()) {
				m_cursorIndex++;
				m_currentBlinkTimeSeconds = time * 1.5;
				if ((textScreenSize > textBoundary) && (m_rightmostCharacterIndex < (textScreenSize - 1))
					&& (m_cursorIndex > m_rightmostCharacterIndex))
				{
					m_leftmostCharacterIndex++;
					m_rightmostCharacterIndex++;
				}
			}
		}
		else if (theKey == VK_BACK && !m_fullText.empty()) {
			if (m_cursorIndex != 0)
			{
				m_cursorIndex--;
				if (m_fullText.size() <= 1)
				{
					m_fullText = std::string("");
					m_leftmostCharacterIndex = 0;
					m_rightmostCharacterIndex = 0;
				}

				else
				{
					m_fullText.erase(m_fullText.begin() + m_cursorIndex);
					aStr = m_fullText;
					if ((textScreenSize > textBoundary && (m_leftmostCharacterIndex > 0)))
						m_leftmostCharacterIndex--;
					//m_rightmostCharacterIndex++;
					m_rightmostCharacterIndex--;
				}
			}
		}
		else
		{
			std::stringstream addStr;
			addStr << theKey;

			m_fullText.insert(m_cursorIndex, addStr.str());
			m_currentBlinkTimeSeconds = time * 1.5;
			m_cursorIndex++;
			if (textScreenSize > textBoundary)
			{
				if (m_cursorIndex == (m_rightmostCharacterIndex + 1))
				{
					m_leftmostCharacterIndex++;
					m_rightmostCharacterIndex++;
				}
				else
				{
					//m_rightmostCharacterIndex--;
				}

			}
			else
			{
				m_rightmostCharacterIndex++;
			}
		}

		if (!aStr.empty())
		{
			m_fullText = aStr;
		}
	}
}

WidgetBase* EditLineWidget::Create(const TiXmlNode* data)
{
	EditLineWidget* sw = new EditLineWidget();
	sw->ParseAndAddEvents(data);
	return sw;
}
