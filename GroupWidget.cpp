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
	for (auto& childWidget : m_children) { 

		if (childWidget->m_currentState == UI_STATE_HIDDEN) //Don't handle mouse events for hidden widgets
			continue;

		GroupWidget* asGW = dynamic_cast<GroupWidget*>(childWidget);
		if (asGW) {
			asGW->OnMouseEvent(me);
		}
		else {
			Vec2 size;
			Vec2 childPos = childWidget->GetWorldPosition(); //A future optimization: Subtract local offset of this from the mouse event when recursing down to the next level. Reduces n^2 to n

			childWidget->GetPropertyForCurrentState("size", size);

			Vec2 maxBounds = Vec2(size.x() + childPos.x(), size.y() + childPos.y());

			if (childPos.x() <= mouseCoord.x() && childPos.y() <= mouseCoord.y() && mouseCoord.x() <= maxBounds.x() && mouseCoord.y() <= maxBounds.y()) {

				if ((GetKeyState(VK_LBUTTON) & 0x100) != 0) {
					std::string eventToFire;
					childWidget->GetPropertyForCurrentState("pressed event", eventToFire);
					FireEvent(eventToFire);
				}

				childWidget->OnMouseFocusEvent(me);
			}
			else if (childWidget->m_currentState != UI_STATE_DEFAULT) {
				childWidget->OnMouseUnfocusEvent(me);
			}
		}
	}
}


// #Eiserloh: Is this "Keyboard event" a key-down event, key-up, or WM_CHAR character event?
void GroupWidget::OnKeyBoardEvent(unsigned char theKey)
{
	for (auto& wb : m_children) {

		if (wb->m_currentState == UI_STATE_HIDDEN) //Don't handle mouse events for hidden widgets
			continue;

		if (wb->m_currentState == UI_STATE_DISABLED)
			continue;

		wb->OnKeyboardEvent(theKey);
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

