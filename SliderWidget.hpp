#pragma once
#include "WidgetBase.hpp"

class TiXmlNode;
class SliderWidget :
	public WidgetBase
{
public:
	SliderWidget();
	virtual ~SliderWidget();

	std::string GetNextDecrementID();
	virtual void Render();
	virtual void Update(double deltaTimeSeconds);

	virtual void OnMouseFocusEvent(MouseEvent me);
	virtual void OnMouseUnfocusEvent(MouseEvent me);

	static WidgetBase* Create(const TiXmlNode* data);
	void UpdateHandleSizeFrac();

protected:
	float m_buttonStartOffset;
	float m_buttonEndOffset;
	float m_innerHandleClickFraction;
	float m_buttonSize;
	Vec2 m_worldPosition;
	float m_maxMovementDistance;
	float m_curHandleOffset;

	bool m_firstUpdate;
	bool m_continousFire;
	double m_firingRateTracker;
};

