#include "GroupWidget.hpp"
#include "UISystem.hpp"

namespace {
	bool _ = UISystem::RegisterWidget("GroupWidget", GroupWidget::Create);
};

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

	for (auto& wb : m_children) {
		Vec2 size;
		Vec2 wp = wb->GetWorldPosition(); //A future optimization: Subtract local offset of this from the mouse event when recursing down to the next level. Reduces n^2 to n

		wb->GetPropertyForCurrentState("size", size);

		Vec2 maxBounds = Vec2(size.x() + wp.x(), size.y() + wp.y());

		if (wp.x() >= me.m_cursorPos.x() && wp.y() >= me.m_cursorPos.y() && wp.x() <= maxBounds.x() && wp.y() <= maxBounds.y()) {
			wb->OnMouseEvent(me);
		}
	}
}

WidgetBase* GroupWidget::Create(const TiXmlNode* data)
{
	GroupWidget* gw = new GroupWidget();
	for (const TiXmlNode* widgetDefinition = data->FirstChild("Children"); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
	{
		UISystem::CreateWidgetInParent(gw, widgetDefinition);
	}

	return gw;
}
