#include "UISystem.hpp"
#include "NamedProperties.hpp"
#include "WidgetBase.hpp"
#include "Assert.hpp"
#include "Engine/Libraries/tinyxml.h"

UISystem::UISystem()
{

	TiXmlDocument doc("Data/default.xml");
	doc.LoadFile();

	TiXmlHandle docHandle(&doc);

	//Get the style name and insert it into the namedproperties
	TiXmlHandle styleNameHandle = docHandle.FirstChild("Style");
	TiXmlElement* styleNameElement = styleNameHandle.ToElement();
	const std::string styleName = styleNameElement->Attribute("name");
	FATAL_ASSERT(styleName != "");

	NamedProperties styleProperties;
	styleProperties.Set("name", styleName);

	//Iterate over the types of widgets
	for (const TiXmlNode* widgetDefinition = styleNameElement->FirstChild(); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
	{
		WidgetBase* widget = nullptr;
		const char* widgetName = widgetDefinition->ToElement()->Value();

		if (strcmp(widgetName, "BaseWidget") == 0) {
			widget = new WidgetBase();
		}

		//Iterate over each state definition for the widget type
		for (const TiXmlNode* stateDefinition = widgetDefinition->FirstChild(); stateDefinition; stateDefinition = stateDefinition->NextSibling())
		{
			std::vector<const TiXmlNode*> widgetAttributes = ExtractWidgetAttributesFromStateDefinition(stateDefinition);

		}
	}


	


}

RGBA UISystem::ExtractRGBAFromXML(const TiXmlElement* rgba) {

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



	}

	return widgetAttributes;
}

void UISystem::CreateStyle(const NamedProperties& styleDescriptor) {
	std::string styleName;
	PropertyGetResult nameGetResult = styleDescriptor.Get<std::string>("name", styleName);
	FATAL_ASSERT(nameGetResult == RESULT_SUCCESS);
	m_styles[styleName] = styleDescriptor;
}

void UISystem::Render() {
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		WidgetBase* widget = *it;
		widget->Render();
	}
}

void UISystem::Update(double deltaTimeSeconds)
{
	for (auto it = m_widgets.begin(); it != m_widgets.end(); ++it) {
		WidgetBase* widget = *it;
		widget->Update(deltaTimeSeconds);
	}
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


UISystem::~UISystem()
{
}
