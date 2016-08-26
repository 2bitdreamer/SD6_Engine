#include "WidgetBase.hpp"
#include "Graphics\Renderer.hpp"
#include "UISystem.hpp"
#include "WidgetStyle.hpp"
#include <functional>
#include <vcruntime_typeinfo.h>
#include "KeyFrameAnimationBase.hpp"

int WidgetBase::s_currentID;

WidgetBase::WidgetBase() :
	m_parentWidget(nullptr),
	m_currentState(UI_STATE_DEFAULT)
{
	//#TODO: should probably have different defaults for different states.
	AddPropertyForState("offset", UI_STATE_ALL, Vec2(0.f, 0.f));
	AddPropertyForState("size", UI_STATE_ALL, Vec2(200.f, 125.f));
	AddPropertyForState("border color", UI_STATE_ALL, RGBA(50, 50, 50, 255));
	AddPropertyForState("border size", UI_STATE_ALL, 5.f);
	AddPropertyForState("opacity", UI_STATE_ALL, 1.f);
	AddPropertyForState("color", UI_STATE_ALL, RGBA(255, 255, 255, 255));
	AddPropertyForState("name", UI_STATE_ALL, GetNextDecrementID());

	SetStaticPropertyForState("color", UI_STATE_DISABLED, RGBA(100, 100, 100, 255));

}

WidgetBase::~WidgetBase()
{
}

std::string WidgetBase::GetNextDecrementID() {
	int nextID = WidgetBase::s_currentID;
	std::stringstream ss;
	ss << "Widget" << nextID;
	s_currentID++;
	return ss.str();
}

std::string WidgetBase::GetNameForState(UIState state) {
	switch (state) {
	case UI_STATE_DISABLED:
		return "Disabled";
		break;
	case UI_STATE_PRESSED:
		return "Pressed";
		break;
	case UI_STATE_HIGHLIGHTED:
		return "Highlighted";
		break;
	case UI_STATE_DEFAULT:
		return "Default";
		break;
	 default:
		return "Invalid";
		break;
	}
}

UIState WidgetBase::GetStateForName(const std::string& name)
{
	if (name == "Disabled")
		return UI_STATE_DISABLED;
	if (name == "Pressed")
		return UI_STATE_PRESSED;
	if (name == "Highlighted")
		return UI_STATE_HIGHLIGHTED;
	if (name == "All")
		return UI_STATE_ALL;
	return UI_STATE_DEFAULT;
}

void WidgetBase::ApplyWidgetProperties(const NamedProperties& widgetDescriptor)
{
	for (UIState state = UI_STATE_DEFAULT; state < NUM_UI_STATES; state = (UIState)(state + 1)) {
		std::string stateName = GetNameForState((UIState)state);
		NamedProperties currentNP;
		PropertyGetResult currentGetResult = widgetDescriptor.Get(stateName, currentNP);

		if (currentGetResult == RESULT_SUCCESS) {
			CopyStatePropertyToWidget(state, currentNP);
		}
	}
}


void WidgetBase::OnMouseFocusEvent(MouseEvent me) {
	(void)me;
}

Vec2 WidgetBase::GetWorldPosition()
{
	Vec2 offset;
	GetPropertyForCurrentState("offset", offset);
	if (!m_parentWidget)
		return offset;
	else
		return m_parentWidget->GetWorldPosition() + offset;
}

float WidgetBase::GetOpacity() {
	float opacity;
	GetPropertyForCurrentState("opacity", opacity);
	if (!m_parentWidget)
		return opacity;
	else
		return m_parentWidget->GetOpacity() * opacity;
}

void WidgetBase::Update(double deltaTimeSeconds)
{
	float deltaTimeFloat = (float)deltaTimeSeconds;

	UpdateProperty<RGBA>("border color", deltaTimeFloat);
	UpdateProperty<RGBA>("color", deltaTimeFloat);
	UpdateProperty<Vec2>("size", deltaTimeFloat);
	UpdateProperty<Vec2>("offset", deltaTimeFloat);
	UpdateProperty<float>("opacity", deltaTimeFloat);
	UpdateProperty<float>("border size", deltaTimeFloat);

	ProcessUpdateEvent();
}

void WidgetBase::ProcessUpdateEvent() {
	std::vector<std::string> updateEvent;
	GetPropertyForCurrentState("update event", updateEvent);
	if (!updateEvent.empty()) {
		for (auto it = updateEvent.begin(); it != updateEvent.end(); ++it) {
			FireEvent(*it);
		}
	}
}

void WidgetBase::ProcessRenderEvent() {
	std::vector<std::string> renderEvents;
	GetPropertyForCurrentState("render event", renderEvents);
	if (!renderEvents.empty()) {
		for (auto it = renderEvents.begin(); it != renderEvents.end(); ++it) {
			FireEvent(*it);
		}		
	}
}



void WidgetBase::RenderOutline(const Vec2& worldPos, const Vec2& widgetSize, float lineWidth)
{
	glPushAttrib(GL_LINE_BIT);
	glLineWidth(lineWidth);
	float opacity = GetOpacity();

	RGBA edgeColor;
	Vertex baseOutlineVertex;
	Vertex outlineVertices[4];
	GetPropertyForCurrentState("border color", edgeColor);

	edgeColor.a() = static_cast<unsigned char>(edgeColor.a() * opacity);

	baseOutlineVertex.m_color = edgeColor;

	Vertex bottomLeftOutlineVertex = baseOutlineVertex;
	Vertex topLeftOutlineVertex = baseOutlineVertex;
	Vertex topRightOutlineVertex = baseOutlineVertex;
	Vertex bottomRightOutlineVertex = baseOutlineVertex;

	bottomLeftOutlineVertex.m_position = worldPos;
	topLeftOutlineVertex.m_position = worldPos + Vec2(0.f, widgetSize.y());
	topRightOutlineVertex.m_position = worldPos + widgetSize;
	bottomRightOutlineVertex.m_position = worldPos + Vec2(widgetSize.x(), 0.f);

	outlineVertices[0] = bottomLeftOutlineVertex;
	outlineVertices[1] = topLeftOutlineVertex;
	outlineVertices[2] = topRightOutlineVertex;
	outlineVertices[3] = bottomRightOutlineVertex;

	Renderer& renderer = Renderer::GetInstance();
	renderer.RenderPrimitives(GL_LINE_LOOP, outlineVertices, 4);
	glPopAttrib();
}

void WidgetBase::RenderBackground(const Vec2& worldPos, const Vec2& size, const RGBA& backgroundColor)
{
	Vertex baseVertex;
	Vertex verticesToRender[4];

	baseVertex.m_color = backgroundColor;

	Vertex bottomLeftVertex = baseVertex;
	Vertex topLeftVertex = baseVertex;
	Vertex topRightVertex = baseVertex;
	Vertex bottomRightVertex = baseVertex;

	bottomLeftVertex.m_position = worldPos;
	topLeftVertex.m_position = worldPos + Vec2(0.f, size.y());
	topRightVertex.m_position = worldPos + size;
	bottomRightVertex.m_position = worldPos + Vec2(size.x(), 0.f);

	verticesToRender[0] = bottomLeftVertex;
	verticesToRender[1] = topLeftVertex;
	verticesToRender[2] = topRightVertex;
	verticesToRender[3] = bottomRightVertex;

	Renderer& renderer = Renderer::GetInstance();
	renderer.RenderPrimitives(GL_QUADS, verticesToRender, 4);
}

WidgetBase* WidgetBase::Create(const TiXmlNode* data)
{
	WidgetBase* wb = new WidgetBase();
	wb->ParseAndAddEvents(data);
	return wb;
}

void WidgetBase::ApplyGeneralStyleToAll(WidgetStyle* baseStyle)
{
	if (!baseStyle)
		return;

	auto properties = baseStyle->GetProperties();

	if (properties.empty())
		return;

	auto it = properties.begin();
	State st = it->first;
	const NamedProperties& np = it->second;

	for (UIState sta = UI_STATE_DEFAULT; sta < NUM_UI_STATES; sta = (UIState)(sta + 1)) {
		CopyStatePropertyToWidget(sta, np);
	}
}

void WidgetBase::ApplyStyle(WidgetStyle* baseStyle)
{
	if (!baseStyle)
		return;

	auto properties = baseStyle->GetProperties();

	bool first = true;
	for (auto it = properties.begin(); it != properties.end(); ++it) {
		if (!first)
		{
			State st = it->first;
			const NamedProperties& np = it->second;

			if (st.type == STATE_NONE) {
				CopyStatePropertyToWidget(st.state1, np);
			}
		}
		else
			first = false;
	}
}

void WidgetBase::OnKeyboardEvent(unsigned char theKey)
{
	(void)theKey;
}

void WidgetBase::OnMouseUnfocusEvent(MouseEvent me)
{
	(void)me;
}

void WidgetBase::Render()
{
	Vec2 worldPos = GetWorldPosition();

	float opacity = GetOpacity();

	RGBA backgroundColor;
	GetPropertyForCurrentState("color", backgroundColor);
	backgroundColor.a() = static_cast<unsigned char>(backgroundColor.a() * opacity);

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float borderSize;
	GetPropertyForCurrentState("border size", borderSize);

	RenderBackground(worldPos, size, backgroundColor);
	RenderOutline(worldPos, size, borderSize);
}


void WidgetBase::SwitchState(UIState newState)
{
	if (m_currentState == newState)
		return;

	if (m_currentState == UI_STATE_DISABLED && (newState == UI_STATE_HIGHLIGHTED || newState == UI_STATE_PRESSED)) //Don't let disabled buttons become highlighted or pressed
		return;

	NamedProperties& thisProp = m_stateProperties[newState];

	auto& thisPropMap = thisProp.GetPropertyMap();

	for (auto it = thisPropMap.begin(); it != thisPropMap.end(); ++it) {
 		const std::string name = it->first;
 		KeyFrameAnimationBase& kfa = thisProp.Get<KeyFrameAnimationBase>(name);
 
  		if (!kfa.IsLooping()) {
  			kfa.SetAnimationTime(0.f);
  		}
	}

	std::vector<std::string> exitEvent;
	GetPropertyForCurrentState("exit event", exitEvent);
	if (!exitEvent.empty()) {
		for (auto it = exitEvent.begin(); it != exitEvent.end(); ++it) {
			FireEvent(*it);
		}
	}

	m_currentState = newState;

	std::vector<std::string> enterEvent;
	GetPropertyForCurrentState("enter event", enterEvent);
	if (!enterEvent.empty()) {
		for (auto it = enterEvent.begin(); it != enterEvent.end(); ++it) {
			FireEvent(*it);
		}
	}
}

void WidgetBase::ParseAndAddEvents(const TiXmlNode* data) {
	if (data) {
		const char* updateEventToFire = data->ToElement()->Attribute("OnUpdate");
		const char* renderEventToFire = data->ToElement()->Attribute("OnRender");
		const char* clickEventToFire = data->ToElement()->Attribute("OnClick");
		if (updateEventToFire) {
			SetStaticPropertyForState("update event", UI_STATE_ALL, SplitString(updateEventToFire, ","));
		}
		if (renderEventToFire) {
			SetStaticPropertyForState("render event", UI_STATE_ALL, SplitString(renderEventToFire, ","));
		}
		if (clickEventToFire) {
			SetStaticPropertyForState("click event", UI_STATE_PRESSED, SplitString(clickEventToFire, ","));
		}
	}
}

void WidgetBase::CopyStatePropertyToWidget(UIState state, const NamedProperties& currentNP)
{
			KeyFrameAnimation<Vec2> offset;
			KeyFrameAnimation<Vec2> size;
			KeyFrameAnimation<RGBA> color;
			KeyFrameAnimation<RGBA> edgeColor;
			KeyFrameAnimation<RGBA> innerColor;
			KeyFrameAnimation<float> borderSize;
			KeyFrameAnimation<float> opacity;
			KeyFrameAnimation<RGBA> textColor;
			KeyFrameAnimation<float> textScale;
			KeyFrameAnimation<float> textOpacity;
			KeyFrameAnimation<float> duration;
			KeyFrameAnimation<std::string> textContents;
			KeyFrameAnimation<std::vector<std::string>> updateEvent;
			KeyFrameAnimation<std::vector<std::string>> renderEvent;
			KeyFrameAnimation<std::vector<std::string>> enterEvent;
			KeyFrameAnimation<std::vector<std::string>> exitEvent;


			PropertyGetResult ofr = currentNP.Get("offset", offset);
			PropertyGetResult sr = currentNP.Get("size", size);
			PropertyGetResult cr = currentNP.Get("color", color);
			PropertyGetResult opr = currentNP.Get("opacity", opacity);
			PropertyGetResult ec = currentNP.Get("border color", edgeColor);
			PropertyGetResult bs = currentNP.Get("border size", borderSize);
			PropertyGetResult tc = currentNP.Get("text color", textColor);
			PropertyGetResult ts = currentNP.Get("text scale", textScale);
			PropertyGetResult to = currentNP.Get("text opacity", textOpacity);
			PropertyGetResult tcon = currentNP.Get("text", textContents);
			PropertyGetResult innerCol = currentNP.Get("inner color", innerColor);
			PropertyGetResult updEv = currentNP.Get("update event", updateEvent);
			PropertyGetResult renEv = currentNP.Get("render event", renderEvent);
			PropertyGetResult enEv = currentNP.Get("enter event", enterEvent);
			PropertyGetResult exEv = currentNP.Get("exit event", exitEvent);
			PropertyGetResult dur = currentNP.Get("duration", duration);

			if (ofr == RESULT_SUCCESS)
				m_stateProperties[state].Set("offset", offset);

			if (sr == RESULT_SUCCESS)
				m_stateProperties[state].Set("size", size);

			if (cr == RESULT_SUCCESS)
				m_stateProperties[state].Set("color", color);

			if (opr == RESULT_SUCCESS)
				m_stateProperties[state].Set("opacity", opacity);

			if (ec == RESULT_SUCCESS)
				m_stateProperties[state].Set("border color", edgeColor);

			if (bs == RESULT_SUCCESS)
				m_stateProperties[state].Set("border size", borderSize);

			if (tc == RESULT_SUCCESS)
				m_stateProperties[state].Set("text color", textColor);

			if (ts == RESULT_SUCCESS)
				m_stateProperties[state].Set("text scale", textScale);

			if (to == RESULT_SUCCESS)
				m_stateProperties[state].Set("text opacity", textOpacity);

			if (tcon == RESULT_SUCCESS)
				m_stateProperties[state].Set("text", textContents);

			if (innerCol == RESULT_SUCCESS)
				m_stateProperties[state].Set("inner color", innerColor);

			if (updEv == RESULT_SUCCESS)
				m_stateProperties[state].Set("update event", updateEvent);

			if (renEv == RESULT_SUCCESS)
				m_stateProperties[state].Set("render event", renderEvent);

			if (enEv == RESULT_SUCCESS)
				m_stateProperties[state].Set("enter event", enterEvent);

			if (exEv == RESULT_SUCCESS)
				m_stateProperties[state].Set("exit event", exitEvent);

			if (dur == RESULT_SUCCESS)
				m_stateProperties[state].Set("duration", duration);
}
