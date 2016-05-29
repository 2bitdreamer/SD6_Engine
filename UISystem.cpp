#include "UISystem.hpp"
#include "NamedProperties.hpp"
#include "WidgetBase.hpp"
#include "Assert.hpp"
#include "GroupWidget.hpp"
#include "Engine/Libraries/tinyxml.h"
#include <algorithm>
#include "WidgetStyle.hpp"

UISystem::UISystem()
{
	
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

WidgetBase* UISystem::CreateStyledWidget(const std::string& widgetType, const std::string& styleName) {
	WidgetStyle* baseStyle = s_styles[styleName][widgetType];
	WidgetBase* wid;

	wid->ApplyStyle(baseStyle);
	return wid;
}

void UISystem::ReadWidgetFile(const std::string& filePath) {
	TiXmlDocument doc(filePath.c_str());
	doc.LoadFile();
	TiXmlHandle docHandle(&doc);

	for (const TiXmlNode* widgetDefinition = docHandle.ToElement()->FirstChild(); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
	{

		CreateWidgetInParent(m_rootWidget, widgetDefinition);

		//#TODO: Handle GroupWidgets and such
		//ReadWidgetFile delegates itself to a function that takes a GroupWidget(parent) and a TiXMLNode* 
	}
}

void UISystem::CreateWidgetInParent(GroupWidget* parent, const TiXmlNode* data) {
	const char* widgetName = data->ToElement()->Value();
	const char* styleName = data->ToElement()->Attribute("style");

	if (!styleName)
		styleName = "Default";

	WidgetBase* wb = CreateStyledWidget(widgetName, styleName);
	WidgetStyle style = WidgetStyle(data);
	wb->ApplyStyle(&style);

	parent->m_children.push_back(wb);
}


NamedProperties UISystem::ExtractWidgetAttributesFromStateDefinition(const TiXmlNode* stateDefinition) {
	NamedProperties widgetAttributes;


	const TiXmlElement* bgcolor = stateDefinition->FirstChild("BackgroundColor")->ToElement();
	const TiXmlElement* bordersize = stateDefinition->FirstChild("BorderSize")->ToElement();
	const TiXmlElement* borderColor = stateDefinition->FirstChild("BorderColor")->ToElement();
	const TiXmlElement* size = stateDefinition->FirstChild("Size")->ToElement();
	const TiXmlElement* offset = stateDefinition->FirstChild("Offset")->ToElement();
	const TiXmlElement* opacity = stateDefinition->FirstChild("Opacity")->ToElement();


	if (bgcolor) {
		const char* R = bgcolor->Attribute("R");
		const char* G = bgcolor->Attribute("G");
		const char* B = bgcolor->Attribute("B");
		const char* A = bgcolor->Attribute("A");

		if (!A)
			A = "255";

		char Rc = (char)atoi(R);
		char Gc = (char)atoi(G);
		char Bc = (char)atoi(B);
		char Ac = (char)atoi(A);

		widgetAttributes.Set("color", RGBA(Rc, Gc, Bc, Ac));
	}

	if (size) {
		const char* sizeX = size->Attribute("X");
		const char* sizeY = size->Attribute("Y");

		float X = atof(sizeX);
		float Y = atof(sizeY);

		widgetAttributes.Set("size", Vec2(X, Y));
	}

	if (offset) {
		const char* offsetX = offset->Attribute("X");
		const char* offsetY = offset->Attribute("Y");

		float X = atof(offsetX);
		float Y = atof(offsetY);

		widgetAttributes.Set("offset", Vec2(X, Y));

	}

	if (opacity) {
		

	}

	return widgetAttributes;
}

void UISystem::Render() {
	m_rootWidget->Render();
}

void UISystem::OnMouseEvent(MouseEvent me) {

}

void UISystem::Update(double deltaTimeSeconds)
{
	m_rootWidget->Update(deltaTimeSeconds);
}



WidgetBase* UISystem::CreateWidget(const NamedProperties& widgetDescriptor) {

	std::string widgetTypeName;
	widgetDescriptor.Get("WidgetTypeName", widgetTypeName);

	WidgetBase* widget = nullptr;

	if (widgetTypeName == "WidgetBase")
	 widget = new WidgetBase(); //#TODO: add actual widget types when i have them

	widget->ApplyWidgetProperties(widgetDescriptor);

	return widget;
}

bool UISystem::RegisterWidget(const std::string& name, std::function<WidgetBase*(const TiXmlNode*)> creationFunc)
{
	return s_widgetFactory.insert(std::make_pair(name, creationFunc)).second;
}





UISystem::~UISystem()
{
}
