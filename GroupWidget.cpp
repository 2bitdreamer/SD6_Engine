#include "GroupWidget.hpp"
#include "UISystem.hpp"

GroupWidget::GroupWidget() : WidgetBase()
{
}


GroupWidget::~GroupWidget()
{
}

void GroupWidget::Update(double deltaTimeSeconds)
{
	for (auto& wb : m_children) {
		if (wb->m_currentState != UI_STATE_HIDDEN) {
			wb->Update(deltaTimeSeconds);
		}
	}
}

void GroupWidget::Render()
{
	for (auto& wb : m_children) {
		if (wb->m_currentState != UI_STATE_HIDDEN) {
			wb->Render();
		}
	}
}

void GroupWidget::OnMouseEvent(MouseEvent me) {
	//Has click-through problem
	if (m_currentState == UI_STATE_DISABLED)
		return;

	Vec2 mouseCoord = me.m_cursorPos;
	for (auto& wb : m_children) {

		if (wb->m_currentState == UI_STATE_HIDDEN) //Don't handle mouse events for hidden widgets
			continue;

		GroupWidget* asGW = dynamic_cast<GroupWidget*>(wb);
		if (asGW) {
			asGW->OnMouseEvent(me);
		}
		else {
			Vec2 size;
			Vec2 wp = wb->GetWorldPosition(); //A future optimization: Subtract local offset of this from the mouse event when recursing down to the next level. Reduces n^2 to n

			wb->GetPropertyForCurrentState("size", size);

			Vec2 maxBounds = Vec2(size.x() + wp.x(), size.y() + wp.y());

			if (wp.x() <= mouseCoord.x() && wp.y() <= mouseCoord.y() && mouseCoord.x() <= maxBounds.x() && mouseCoord.y() <= maxBounds.y()) {

				if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) {
					std::string eventToFire;
					wb->GetPropertyForCurrentState("pressed event", eventToFire);
					FireEvent(eventToFire);
				}

				wb->OnMouseFocusEvent(me);
			}
			else if (wb->m_currentState != UI_STATE_DEFAULT) {
				wb->OnMouseUnfocusEvent(me);
			}
		}
	}
}



void GroupWidget::OnKeyBoardEvent(unsigned char theKey)
{
	for (auto& wb : m_children) {

		if (wb->m_currentState == UI_STATE_HIDDEN) //Don't handle mouse events for hidden widgets
			continue;

		wb->OnKeyBoardEvent(theKey);
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

	gw->ParseAndAddEvents(data);
	return gw;
}

void GroupWidget::DestroyWidget(WidgetBase* wb)
{
	for (auto it = m_children.begin(); it != m_children.end(); ) {
		WidgetBase* child = *it;
		GroupWidget* asGW = dynamic_cast<GroupWidget*>(child);
		if (child == wb) {
			it = m_children.erase(it);
			return;
		}
		else if (asGW) {
			asGW->DestroyWidget(wb);
		}
		else {
			++it;
		}
	}
}

