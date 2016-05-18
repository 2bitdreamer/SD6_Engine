#include "WidgetBase.hpp"
#include "Graphics\Renderer.hpp"



WidgetBase::WidgetBase() :
	m_parentWidget(nullptr),
	m_currentState(UI_STATE_DEFAULT)
{
	//#TODO: should probably have different defaults for different states.
	SetPropertyForState("offset", UI_STATE_ALL, Vec2(0.f, 0.f));
	SetPropertyForState("size", UI_STATE_ALL, Vec2(100.f, 50.f));
	SetPropertyForState("edge color", UI_STATE_ALL, RGBA(200, 200, 200, 255));
	SetPropertyForState("opacity", UI_STATE_ALL, RGBA(10, 10, 10, 255));
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

void WidgetBase::ApplyWidgetProperties(const NamedProperties& widgetDescriptor) {

	CopyPropertyToWidget( "offset", widgetDescriptor );
	CopyPropertyToWidget("size", widgetDescriptor );
	CopyPropertyToWidget("color", widgetDescriptor );
	CopyPropertyToWidget("opacity", widgetDescriptor );

}

Vec2 WidgetBase::GetWorldPosition()
{
	Vec2 offset;
	GetProperty("offset", offset);
	if (!m_parentWidget)
		return offset;
	else
		return m_parentWidget->GetWorldPosition() + offset;
}

float WidgetBase::GetOpacity() {
	float opacity;
	GetProperty("opacity", opacity);
	if (!m_parentWidget)
		return opacity;
	else
		return m_parentWidget->GetOpacity() * opacity;
}

void WidgetBase::Update(double deltaTimeSeconds)
{
	float deltaTimeFloat = (float)deltaTimeSeconds;

	UpdateProperty<RGBA>("edge color", deltaTimeFloat);
	UpdateProperty<RGBA>("color", deltaTimeFloat);
	UpdateProperty<Vec2>("size", deltaTimeFloat);
	UpdateProperty<Vec2>("offset", deltaTimeFloat);
	UpdateProperty<float>("opacity", deltaTimeFloat);
}

void WidgetBase::RenderOutline(const Vec2& worldPos, const Vec2& size)
{
	float opacity = GetOpacity();

	RGBA edgeColor;
	Vertex baseOutlineVertex;
	Vertex outlineVertices[4];
	GetProperty("edge color", edgeColor);
	edgeColor.a() *= opacity;
	baseOutlineVertex.m_color = edgeColor;

	Vertex bottomLeftOutlineVertex = baseOutlineVertex;
	Vertex topLeftOutlineVertex = baseOutlineVertex;
	Vertex topRightOutlineVertex = baseOutlineVertex;
	Vertex bottomRightOutlineVertex = baseOutlineVertex;

	bottomLeftOutlineVertex.m_position = worldPos;
	topLeftOutlineVertex.m_position = worldPos + Vec2(0.f, size.y());
	topRightOutlineVertex.m_position = worldPos + size;
	bottomLeftOutlineVertex.m_position = worldPos + Vec2(size.x(), 0.f);

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
	GetProperty("color", backgroundColor);
	backgroundColor.a() *= opacity;

	baseVertex.m_color = backgroundColor;

	Vertex bottomLeftVertex = baseVertex;
	Vertex topLeftVertex = baseVertex;
	Vertex topRightVertex = baseVertex;
	Vertex bottomRightVertex = baseVertex;

	bottomLeftVertex.m_position = worldPos;
	topLeftVertex.m_position = worldPos + Vec2(0.f, size.y());
	topRightVertex.m_position = worldPos + size;
	bottomLeftVertex.m_position = worldPos + Vec2(size.x(), 0.f);

	verticesToRender[0] = bottomLeftVertex;
	verticesToRender[1] = topLeftVertex;
	verticesToRender[2] = topRightVertex;
	verticesToRender[3] = bottomRightVertex;

	Renderer& renderer = Renderer::GetInstance();
	renderer.RenderPrimitives(GL_QUADS, verticesToRender, 4);
}

void WidgetBase::Render()
{
	Vec2 worldPos = GetWorldPosition();

	Vec2 size;
	GetProperty("size", size);

	RenderBackground(worldPos, size);
	RenderOutline(worldPos, size);
}

void WidgetBase::CopyPropertyToWidget(const std::string& propertyName, const NamedProperties& widgetDescriptor)
{
	for (int state = 0; state < NUM_UI_STATES; state++) {
		std::string stateName = GetNameForState((UIState)state);
		NamedProperties currentNP;
		PropertyGetResult currentGetResult = widgetDescriptor.Get(stateName, currentNP);

		if (currentGetResult == RESULT_SUCCESS) {
			Vec2 offset;
			Vec2 size;
			RGBA color;
			RGBA edgeColor;
			float opacity;

			PropertyGetResult ofr = currentNP.Get("offset", offset);
			PropertyGetResult sr = currentNP.Get("size", size);
			PropertyGetResult cr = currentNP.Get("color", color);
			PropertyGetResult opr = currentNP.Get("opacity", opacity);

			if (ofr == RESULT_SUCCESS)
				m_stateProperties[state].Set("offset", offset);

			if (sr == RESULT_SUCCESS)
				m_stateProperties[state].Set("size", size);

			if (cr == RESULT_SUCCESS)
				m_stateProperties[state].Set("color", color);

			if (opr == RESULT_SUCCESS)
				m_stateProperties[state].Set("opacity", opacity);
		}
	}
}