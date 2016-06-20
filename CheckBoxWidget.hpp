#pragma once
#include "ButtonWidget.hpp"
class CheckBoxWidget :
	public ButtonWidget
{
public:
	CheckBoxWidget();
	void ToggleChecked();
	bool IsChecked();
	virtual ~CheckBoxWidget();

	static WidgetBase* Create(const TiXmlNode* data);
	virtual void OnMouseFocusEvent(MouseEvent me);
	virtual void OnMouseUnfocusEvent(MouseEvent me);
	virtual void Render();
	virtual void Update(double deltaTimeSeconds);
};

