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
	void OnKeyboardEvent(unsigned char theKey);
	void Update(double deltaTimeSeconds);
	void DestroyWidget(WidgetBase* wb);
	void AddStyle(WidgetStyle* style, const std::string& styleName, const std::vector<std::string>& widgetNameList);
	void ReadWidgetFile(const std::string& filePath);

	static void ToggleWidgetVisibility(WidgetBase* wb);
	static void ToggleWidgetVisibility(const std::string& name);
	static bool RegisterWidget(const std::string& name, std::function<WidgetBase*(const TiXmlNode*)> creationFunc);
	static WidgetBase* CreateWidget(const std::string& widgetType, const TiXmlNode* data);
	static void AddWidgetInParent(GroupWidget* parent, const TiXmlNode* data);
	static WidgetBase* AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, WidgetStyle& widgetStyle, GroupWidget* parent=s_rootWidget, std::string widgetName = "named");
	static WidgetBase* AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, WidgetStyle& widgetStyle, std::string widgetName /*= "named"*/, GroupWidget* parent/*=s_rootWidget*/);
public:
	static GroupWidget* s_rootWidget;
	//StyleName, WidgetType for the strings
	static std::map<std::string, std::map<std::string, WidgetStyle*>> s_styles;
	static std::map<std::string, std::function<WidgetBase*(const TiXmlNode*)> > s_widgetFactory;
	static std::map<std::string, WidgetBase*> s_widgets;
private:
	void ToggleWidgetEnabled(const std::string& name);
	void ToggleWidgetEnabled(WidgetBase* wb);
};

