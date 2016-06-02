#pragma once
#include <map>
#include <string>
#include <vector>
#include "NamedProperties.hpp"
#include "Libraries\tinyxml.h"
#include "WidgetBase.hpp"
#include <functional>
class GroupWidget;
class WidgetStyle;

class UISystem
{
public:
	UISystem();
	void ReadStyleFile(const std::string& filePath);
	~UISystem();

	void Render();
	void OnMouseEvent(MouseEvent me);
	void Update(double deltaTimeSeconds);
	static bool RegisterWidget(const std::string& name, std::function<WidgetBase*(const TiXmlNode*)> creationFunc);
	static WidgetBase* CreateStyledWidget(const std::string& widgetType, const std::string& styleName, const TiXmlNode* data);
	void ReadWidgetFile(const std::string& filePath);
	static void CreateWidgetInParent(GroupWidget* parent, const TiXmlNode* data);
public:
	GroupWidget* m_rootWidget;
	//StyleName, WidgetType for the strings
	static std::map<std::string, std::map<std::string, WidgetStyle*>> s_styles;
	static std::map<std::string, std::function<WidgetBase*(const TiXmlNode*)> > s_widgetFactory;
};

