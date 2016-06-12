#include "UISystem.hpp"
#include "NamedProperties.hpp"
#include "WidgetBase.hpp"
#include "Assert.hpp"
#include "GroupWidget.hpp"
#include "Engine/Libraries/tinyxml.h"
#include <algorithm>
#include "WidgetStyle.hpp"
#include "ButtonWidget.hpp"

std::map<std::string, std::map<std::string, WidgetStyle*>> UISystem::s_styles;
std::map<std::string, std::function<WidgetBase*(const TiXmlNode*)> > UISystem::s_widgetFactory;

namespace {
	bool _1 = UISystem::RegisterWidget("Button", &ButtonWidget::Create);
	bool _2 = UISystem::RegisterWidget("WidgetBase", std::bind(&WidgetBase::Create));
	bool _3 = UISystem::RegisterWidget("Group", &GroupWidget::Create);
};

UISystem::UISystem() :
	m_rootWidget(new GroupWidget())
{
	//Read all styles and widgets 

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

	for (const TiXmlNode* widgetDefinition = styleNameElement->FirstChild(); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
	{
		const char* widgetName = widgetDefinition->ToElement()->Value();
		s_styles[styleName][widgetName] = new WidgetStyle(widgetDefinition);
	}
}


void UISystem::ReadWidgetFile(const std::string& filePath) {
	TiXmlDocument doc(filePath.c_str());
	doc.LoadFile();
	TiXmlHandle docHandle(&doc);

	for (TiXmlElement* widgetDefinition = doc.FirstChildElement(); widgetDefinition != NULL; widgetDefinition = widgetDefinition->NextSiblingElement())
	{
		AddWidgetInParent(m_rootWidget, widgetDefinition);
	}

	//ReadWidgetFile delegates itself to a function that takes a GroupWidget(parent) and a TiXMLNode* 
}

WidgetBase* UISystem::CreateStyledWidget(const std::string& widgetType, const std::string& styleName, const TiXmlNode* data) {
	WidgetStyle* baseStyle = s_styles[styleName][widgetType];
	WidgetBase* wid;

	auto result = s_widgetFactory.find(widgetType);

	FATAL_ASSERT(result != s_widgetFactory.end());
	wid = result->second(data);

	if (baseStyle) {
		wid->ApplyStyle(baseStyle);
	}

	return wid;
}

WidgetBase* UISystem::AddStyledWidgetExplicitly(const std::string& widgetType, const std::string& styleName, const NamedProperties props, GroupWidget* parent) {
	WidgetBase* wb = CreateStyledWidget(widgetType, styleName, nullptr);

	wb->m_parentWidget = parent;
	parent->m_children.push_back(wb);

	return wb;
}

void UISystem::AddWidgetInParent(GroupWidget* parent, const TiXmlNode* data) {
	const char* widgetName = data->ToElement()->Value();
	const char* styleName = data->ToElement()->Attribute("style");

	if (!styleName)
		styleName = "Default";

	WidgetBase* wb = CreateStyledWidget(widgetName, styleName, data);

	WidgetStyle style = WidgetStyle(data);
	wb->ApplyStyle(&style);

	wb->m_parentWidget = parent;
	parent->m_children.push_back(wb);
}


void UISystem::Render() {
	m_rootWidget->Render();
}

void UISystem::OnMouseEvent(MouseEvent me) {
	m_rootWidget->OnMouseEvent(me);
}

void UISystem::Update(double deltaTimeSeconds)
{
	m_rootWidget->Update(deltaTimeSeconds);
}


bool UISystem::RegisterWidget(const std::string& name, std::function<WidgetBase*(const TiXmlNode*)> creationFunc)
{
	return s_widgetFactory.insert(std::make_pair(name, creationFunc)).second;
}





UISystem::~UISystem()
{
}
