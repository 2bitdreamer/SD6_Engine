#pragma once
#include <map>
#include <string>
#include <vector>
class WidgetBase;
class NamedProperties;

class UISystem
{
public:
	UISystem();
	NamedProperties ExtractWidgetAttributesFromStateDefinition(const TiXmlNode* stateDefinition);
	void CreateStyle(const NamedProperties& styleDescriptor);
	void Render();
	void Update(double deltaTimeSeconds);
	WidgetBase* CreateWidget(const NamedProperties& widgetDescriptor);
	~UISystem();

public:
	std::map<std::string, NamedProperties> m_styles;
	std::vector<WidgetBase*> m_widgets;
};

