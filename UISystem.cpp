#include "UISystem.hpp"
#include "NamedProperties.hpp"
#include "WidgetBase.hpp"
#include "Assert.hpp"
#include "GroupWidget.hpp"
#include "Engine/Libraries/tinyxml.h"
#include <algorithm>
#include "WidgetStyle.hpp"
#include "ButtonWidget.hpp"
#include "LabelWidget.hpp"
#include "CheckBoxWidget.hpp"
#include "EventSystem.hpp"
#include "ProgressBarWidget.hpp"

#include "ScrollbarWidget.hpp"
#include "EditLineWidget.hpp"

std::map<std::string, std::map<std::string, WidgetStyle*>> UISystem::s_styles;
std::map<std::string, std::function<WidgetBase*(const TiXmlNode*)> > UISystem::s_widgetFactory;
std::map<std::string, WidgetBase*> UISystem::s_widgets;
GroupWidget* UISystem::s_rootWidget;


void UISystem::ToggleWidgetEnabled(const std::string& name)
{
	auto result = s_widgets.find(name);
	if (result != s_widgets.end()) {
		ToggleWidgetEnabled(result->second);
	}
}

void UISystem::ToggleWidgetEnabled(WidgetBase* wb)
{
	if (wb->m_currentState != UI_STATE_DISABLED) {
		wb->SwitchState(UI_STATE_DISABLED);
	}
	else {
		wb->SwitchState(UI_STATE_DEFAULT);
	}
}

namespace {
	bool _1 = UISystem::RegisterWidget("Button", &ButtonWidget::Create);
	bool _2 = UISystem::RegisterWidget("WidgetBase", &WidgetBase::Create);
	bool _3 = UISystem::RegisterWidget("Group", &GroupWidget::Create);
	bool _4 = UISystem::RegisterWidget("Label", &LabelWidget::Create);
	bool _5 = UISystem::RegisterWidget("Checkbox", &CheckBoxWidget::Create);
	bool _6 = UISystem::RegisterWidget("ProgressBar", &ProgressBarWidget::Create);
	bool _7 = UISystem::RegisterWidget("Slider", &SliderWidget::Create);
	bool _8 = UISystem::RegisterWidget("ScrollBar", &ScrollbarWidget::Create);
	bool _9 = UISystem::RegisterWidget("EditLine", &EditLineWidget::Create);
};



UISystem::UISystem() 
{
	//Read all styles and widgets 

	s_rootWidget = new GroupWidget();

	std::vector<std::string> out_styles;
	FindAllFilesOfType("Data/Styles/", "*", out_styles);

	for (auto it = out_styles.begin(); it != out_styles.end(); ++it) {
		std::string filePath = *it;
		ReadStyleFile(filePath);
	}

	std::vector<std::string> out_widgets;
	FindAllFilesOfType("Data/Widgets/", "*", out_widgets);

	for (auto it = out_widgets.begin(); it != out_widgets.end(); ++it) {
		std::string filePath = *it;
		ReadWidgetFile(filePath);
	}

	WidgetStyle wid;
	NamedProperties defaultProps;
	NamedProperties allProps;

	KeyFrameAnimation<RGBA> animColor;
	animColor.AddAnimationFrameAtParameter(RGBA(50, 255, 50, 255), 0.f);
	//defaultProps.Set("color", animColor);

	KeyFrameAnimation<Vec2> offsetAnim;
	offsetAnim.AddAnimationFrameAtParameter(Vec2(100.f, 500.f), 0.f);
	allProps.Set("offset", offsetAnim);

	KeyFrameAnimation<RGBA> animColor2;
	animColor2.AddAnimationFrameAtParameter(RGBA(255, 0, 0, 255), 0.f);
	allProps.Set("color", animColor2);

	wid.AddProperty(UI_STATE_DEFAULT, defaultProps);
	wid.AddGeneralProperty(allProps);
	WidgetBase* widget = AddStyledWidgetExplicitly("Button", "Default", wid, s_rootWidget);

	std::string name;
	widget->GetPropertyForCurrentState("name", name);

	//ToggleWidgetVisibility(name);
	ToggleWidgetEnabled(name);


}

void UISystem::ReadStyleFile(const std::string& filePath) {
	TiXmlDocument doc(filePath.c_str());
	doc.LoadFile();
	TiXmlHandle docHandle(&doc);

	//Get the style name and insert it into the named properties
	TiXmlHandle styleNameHandle = docHandle.FirstChild("Style");
	TiXmlElement* styleNameElement = styleNameHandle.ToElement();
	const std::string styleName = styleNameElement->Attribute("name");
	FATAL_ASSERT(styleName != "");

	const TiXmlNode* global = styleNameElement->FirstChild("Global");
	bool hasGlobal = global != nullptr;
	WidgetStyle tempStyle;

	if (hasGlobal) {
		tempStyle = WidgetStyle(global);
	}

	for (const TiXmlNode* widgetDefinition = styleNameElement->FirstChild(); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
	{
		const char* widgetName = widgetDefinition->ToElement()->Value();
		if (strcmp(widgetName, "Global") != 0) {
			WidgetStyle ws = tempStyle;
			WidgetStyle* newWs = new WidgetStyle(widgetDefinition);
			ws.MergeStyle(newWs);
			WidgetStyle* mergedWS = new WidgetStyle(ws);

			delete newWs;

			s_styles[styleName][widgetName] = mergedWS;
		}
	}

	auto styleMap = s_styles[styleName];
	for (auto& it : s_widgetFactory) {
		std::string widgetName = it.first;
		auto found = styleMap.find(widgetName);
		if (found == styleMap.end()) {
			s_styles[styleName][widgetName] = new WidgetStyle(tempStyle);
		}
	}
}


void UISystem::ReadWidgetFile(const std::string& filePath) {
	TiXmlDocument doc(filePath.c_str());
	doc.LoadFile();
	TiXmlHandle docHandle(&doc);

	for (TiXmlElement* widgetDefinition = doc.FirstChildElement(); widgetDefinition != NULL; widgetDefinition = widgetDefinition->NextSiblingElement())
	{
		AddWidgetInParent(s_rootWidget, widgetDefinition);
	}

	//ReadWidgetFile delegates itself to a function that takes a GroupWidget(parent) and a TiXMLNode* 
}

WidgetBase* UISystem::CreateWidget(const std::string& widgetType, const TiXmlNode* data) {
	WidgetBase* wid;

	auto result = s_widgetFactory.find(widgetType);

	FATAL_ASSERT(result != s_widgetFactory.end());
	wid = result->second(data);

	return wid;
}

WidgetBase* UISystem::AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, WidgetStyle& widgetStyle, GroupWidget* parent/*=s_rootWidget*/, std::string widgetName /*= "named"*/) {
	WidgetBase* wb = CreateWidget(widgetType, nullptr);

	WidgetStyle* ws = s_styles[styleName][widgetType];
	wb->ApplyGeneralStyleToAll(ws);
	wb->ApplyGeneralStyleToAll(&widgetStyle);
	wb->ApplyStyle(ws);
	wb->ApplyStyle(&widgetStyle);

	if (widgetName == "named") { //Already has a UUID name from code, get it
		wb->GetPropertyForCurrentState("name", widgetName);
	}
	else { //We're naming it explicitly, so set its name
		wb->AddPropertyForState("name", UI_STATE_ALL, widgetName);
	}

	wb->m_parentWidget = parent;
	parent->m_children.push_back(wb);

	s_widgets[widgetName] = wb;
	return wb;
}


WidgetBase* UISystem::AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, WidgetStyle& widgetStyle, std::string widgetName /*= "named"*/, GroupWidget* parent/*=s_rootWidget*/) {
	return AddStyledWidgetExplicitly(widgetType, styleName, widgetStyle, parent, widgetName);
}

void UISystem::AddStyle(WidgetStyle* style, const std::string& styleName, const std::vector<std::string>& widgetNameList) {

	for (auto it = widgetNameList.begin(); it != widgetNameList.end(); ++it) {
		std::string widgetName = *it;
		s_styles[styleName][widgetName] = style;
	}
}

void UISystem::ToggleWidgetVisibility(WidgetBase* wb) {
	if (wb->m_currentState != UI_STATE_HIDDEN) {
		wb->SwitchState(UI_STATE_HIDDEN);
	}
	else {
		wb->SwitchState(UI_STATE_DEFAULT);
	}
}

void UISystem::ToggleWidgetVisibility(const std::string& name) {
	auto result = s_widgets.find(name);
	if (result != s_widgets.end()) {
		ToggleWidgetVisibility(result->second);
	}
}

void UISystem::AddWidgetInParent(GroupWidget* parent, const TiXmlNode* data) {
	const char* widgetType = data->ToElement()->Value();
	const char* styleName = data->ToElement()->Attribute("style");

	if (!styleName)
		styleName = "Default";

	//Construct the basic styled widget. No properties will be applied
	WidgetBase* wb = CreateWidget(widgetType, data);

	//Apply the general (All state) style level properties
	wb->ApplyGeneralStyleToAll(s_styles[styleName][widgetType]);
	//Apply the general (All state) widget level properties
	WidgetStyle style = WidgetStyle(data);
	wb->ApplyGeneralStyleToAll(&style);
	//Apply the state specific style level properties
	wb->ApplyStyle(s_styles[styleName][widgetType]);
	//Apply the state specific widget level properties
	wb->ApplyStyle(&style);

	const char* widgetName = data->ToElement()->Attribute("name");
	std::string nameAsString;
	if (widgetName) {
		nameAsString = std::string(widgetName);
		wb->AddPropertyForState("name", UI_STATE_ALL, nameAsString);
	}
	else {
		wb->GetPropertyForCurrentState("name", nameAsString);
	}

	wb->m_parentWidget = parent;
	parent->m_children.push_back(wb);

	s_widgets[nameAsString] = wb;
}

void UISystem::Render() {
	s_rootWidget->Render();
}

void UISystem::OnMouseEvent(MouseEvent me) {
	s_rootWidget->OnMouseEvent(me);
}

void UISystem::OnKeyboardEvent(unsigned char theKey) {
	s_rootWidget->OnKeyBoardEvent(theKey);
}

void UISystem::Update(double deltaTimeSeconds)
{
	s_rootWidget->Update(deltaTimeSeconds);
}

void UISystem::DestroyWidget(WidgetBase* wb) {
	s_rootWidget->DestroyWidget(wb);

	std::string name;
	wb->GetPropertyForCurrentState("name", name);

	auto result = s_widgets.find(name);
	if (result != s_widgets.end()) {
		s_widgets.erase(result);
		delete result->second;
	}
}


bool UISystem::RegisterWidget(const std::string& name, std::function<WidgetBase*(const TiXmlNode*)> creationFunc)
{
	return s_widgetFactory.insert(std::make_pair(name, creationFunc)).second;
}





UISystem::~UISystem()
{
}
