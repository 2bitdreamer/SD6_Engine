#include "GroupWidget.hpp"
#include "UISystem.hpp"

GroupWidget::GroupWidget()
{
}


GroupWidget::~GroupWidget()
{
}

void GroupWidget::Update(double deltaTimeSeconds)
{
	for (auto& wb : m_children) {
		wb->Update(deltaTimeSeconds);
	}
}

void GroupWidget::Render()
{
	for (auto& wb : m_children) {
		wb->Render();
	}
}

void GroupWidget::OnMouseEvent(MouseEvent me) {
	//Has click-through problem

	Vec2 convertedMouseCoord = Vec2(me.m_cursorPos.x(), SCREEN_HEIGHT - me.m_cursorPos.y());
	for (auto& wb : m_children) {
		Vec2 size;
		Vec2 wp = wb->GetWorldPosition(); //A future optimization: Subtract local offset of this from the mouse event when recursing down to the next level. Reduces n^2 to n

		wb->GetPropertyForCurrentState("size", size);

		Vec2 maxBounds = Vec2(size.x() + wp.x(), size.y() + wp.y());

		if (wp.x() <= convertedMouseCoord.x() && wp.y() <= convertedMouseCoord.y() && convertedMouseCoord.x() <= maxBounds.x() && convertedMouseCoord.y() <= maxBounds.y()) {
			wb->OnMouseEvent(me);
		}
	}
}

WidgetBase* GroupWidget::Create(const TiXmlNode* data)
{
	GroupWidget* gw = new GroupWidget();

	const TiXmlNode* children = data->FirstChild("Children");
	if (children) {
		for (const TiXmlNode* widgetDefinition = children->FirstChild(); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
		{
			UISystem::AddWidgetInParent(gw, widgetDefinition);
		}
	}

	return gw;
}
