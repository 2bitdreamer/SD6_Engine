#pragma once
#include "WidgetBase.hpp"
#include "Libraries\tinyxml.h"
class GroupWidget :
	public WidgetBase
{
public:
	GroupWidget();
	virtual ~GroupWidget();
	
	virtual void Update(double deltaTimeSeconds);
	virtual void Render();
	virtual void OnMouseEvent(MouseEvent me);

	static WidgetBase* Create(const TiXmlNode* data);

public:
	std::vector<WidgetBase*> m_children;
	void DestroyWidget(WidgetBase* wb);
	void OnKeyBoardEvent(unsigned char theKey);
};

