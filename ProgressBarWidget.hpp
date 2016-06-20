#pragma once
#include "WidgetBase.hpp"
class ProgressBarWidget :
	public WidgetBase
{
public:
	ProgressBarWidget();
	virtual ~ProgressBarWidget();

	static WidgetBase* Create(const TiXmlNode* data);
	virtual void Render();
	virtual void Update(double deltaTimeSeconds);
	void ParseDuration(const TiXmlNode* data);
};

