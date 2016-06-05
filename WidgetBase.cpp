#include "WidgetBase.hpp"
#include "Graphics\Renderer.hpp"
#include "UISystem.hpp"
#include "WidgetStyle.hpp"
#include <functional>

WidgetBase::WidgetBase() :
	m_parentWidget(nullptr),
	m_currentState(UI_STATE_DEFAULT)
{
	//#TODO: should probably have different defaults for different states.
	SetPropertyForState("offset", UI_STATE_ALL, Vec2(0.f, 0.f));
	SetPropertyForState("size", UI_STATE_ALL, Vec2(100.f, 50.f));
	SetPropertyForState("border color", UI_STATE_ALL, RGBA(200, 200, 200, 255));
	SetPropertyForState("border size", UI_STATE_ALL, 5.f);
	SetPropertyForState("opacity", UI_STATE_ALL, 1.f);
	SetPropertyForState("color", UI_STATE_HIGHLIGHTED, RGBA(255, 255, 255, 255));
}

WidgetBase::~WidgetBase()
{
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


void WidgetBase::OnMouseEvent(MouseEvent me) {


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
}

void WidgetBase::RenderOutline(const Vec2& worldPos, const Vec2& size)
{
	float opacity = GetOpacity();

	RGBA edgeColor;
	Vertex baseOutlineVertex;
	Vertex outlineVertices[4];
	GetPropertyForCurrentState("border color", edgeColor);
	edgeColor.a() *= opacity;
	baseOutlineVertex.m_color = edgeColor;

	Vertex bottomLeftOutlineVertex = baseOutlineVertex;
	Vertex topLeftOutlineVertex = baseOutlineVertex;
	Vertex topRightOutlineVertex = baseOutlineVertex;
	Vertex bottomRightOutlineVertex = baseOutlineVertex;

	bottomLeftOutlineVertex.m_position = worldPos;
	topLeftOutlineVertex.m_position = worldPos + Vec2(0.f, size.y());
	topRightOutlineVertex.m_position = worldPos + size;
	bottomRightOutlineVertex.m_position = worldPos + Vec2(size.x(), 0.f);

	outlineVertices[0] = bottomLeftOutlineVertex;
	outlineVertices[1] = topLeftOutlineVertex;
	outlineVertices[2] = topRightOutlineVertex;
	outlineVertices[3] = bottomRightOutlineVertex;

	Renderer& renderer = Renderer::GetInstance();
	renderer.RenderPrimitives(GL_LINE_LOOP, outlineVertices, 4);
}

void WidgetBase::RenderBackground(const Vec2& worldPos, const Vec2& size)
{
	Vertex baseVertex;
	Vertex verticesToRender[4];

	float opacity = GetOpacity();

	RGBA backgroundColor;
	GetPropertyForCurrentState("color", backgroundColor);
	backgroundColor.a() *= opacity;

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

WidgetBase* WidgetBase::Create()
{
	return new WidgetBase();
}

void WidgetBase::ApplyStyle(WidgetStyle* baseStyle)
{
	auto properties = baseStyle->GetProperties();

	for (auto it = properties.begin(); it != properties.end(); ++it) {
		State st = it->first;
		const NamedProperties& np = it->second;

		if (st.type == STATE_NONE) {
			CopyStatePropertyToWidget(st.state1, np);
		}
	}
}

void WidgetBase::Render()
{
	Vec2 worldPos = GetWorldPosition();

	Vec2 size;
	GetPropertyForCurrentState("size", size);

	float borderSize;
	GetPropertyForCurrentState("border size", borderSize);

	RenderBackground(worldPos, size);
	RenderOutline(worldPos, size);
}

void WidgetBase::CopyStatePropertyToWidget(UIState state, const NamedProperties& currentNP)
{
			KeyFrameAnimation<Vec2> offset;
			KeyFrameAnimation<Vec2> size;
			KeyFrameAnimation<RGBA> color;
			KeyFrameAnimation<RGBA> edgeColor;
			KeyFrameAnimation<float> borderSize;
			KeyFrameAnimation<float> opacity;
			KeyFrameAnimation<RGBA> textColor;


			PropertyGetResult ofr = currentNP.Get("offset", offset);
			PropertyGetResult sr = currentNP.Get("size", size);
			PropertyGetResult cr = currentNP.Get("color", color);
			PropertyGetResult opr = currentNP.Get("opacity", opacity);
			PropertyGetResult ec = currentNP.Get("border color", edgeColor);
			PropertyGetResult bs = currentNP.Get("border size", borderSize);
			PropertyGetResult tc = currentNP.Get("text color", textColor);

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
}
