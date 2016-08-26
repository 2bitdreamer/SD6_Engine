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
	void OnMouseEvent(const MouseEvent& me); // #Eiserloh: I know these are small, but you should default to "const T&" for all non-primitive argument types in general
	void OnKeyboardEvent(unsigned char theKey);
	void Update(double deltaTimeSeconds);
	void DestroyWidget(WidgetBase* wb);
	void AddStyle(WidgetStyle* style, const std::string& styleName, const std::vector<std::string>& widgetNameList);
	void ReadWidgetFile(const std::string& filePath);

	static void ToggleWidgetVisibility(WidgetBase* wb); // #Eiserloh: wb is non-optional, and you don't check for null, so this should be pass-by-reference
	static void ToggleWidgetVisibility(const std::string& name);
	static bool RegisterWidget(const std::string& name, std::function<WidgetBase*(const TiXmlNode*)> creationFunc);
	static WidgetBase* CreateWidget(const std::string& widgetType, const TiXmlNode* data);
	static void AddWidgetInParent(GroupWidget* parent, const TiXmlNode* data); // #Eiserloh: These pointers are never checked for null, and are non-optional; pass by reference instead to imply this
	static WidgetBase* AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, WidgetStyle& widgetStyle, GroupWidget* parent=s_rootWidget, std::string widgetName = "named");
	static WidgetBase* AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, WidgetStyle& widgetStyle, std::string widgetName /*= "named"*/, GroupWidget* parent/*=s_rootWidget*/);
public:
	static GroupWidget* s_rootWidget;
	//StyleName, WidgetType for the strings
	static std::map<std::string, std::map<std::string, WidgetStyle*>> s_styles; // #Eiserloh: This map-of-maps isn't self-explanatory enough, and the name isn't sufficient (what are the keys in the exterior and interior maps?  I'd have to go look at internal implementation details in order to understand how to use this)
	static std::map<std::string, std::function<WidgetBase*(const TiXmlNode*)> > s_widgetFactory;
	static std::map<std::string, WidgetBase*> s_widgets;
private:
	void ToggleWidgetEnabled(const std::string& name);
	void ToggleWidgetEnabled(WidgetBase* wb); // #Eiserloh: If wb is optional, check on inside... otherwise, pass by reference
};

