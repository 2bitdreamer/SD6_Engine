#include "WidgetStyle.hpp"
#include "UISystem.hpp"


WidgetStyle::WidgetStyle()
{
}


WidgetStyle::WidgetStyle(const TiXmlNode* node)
{
	//Iterate over the types of widgets
	for (const TiXmlNode* widgetDefinition = node->FirstChild(); widgetDefinition; widgetDefinition = widgetDefinition->NextSibling())
	{
		const char* widgetName = widgetDefinition->ToElement()->Value();

		//Iterate over each state definition for the widget type
		for (const TiXmlNode* stateDefinition = widgetDefinition->FirstChild(); stateDefinition; stateDefinition = stateDefinition->NextSibling())
		{
			std::string stateName = stateDefinition->ToElement()->Value();
			UIState state = WidgetBase::GetStateForName(stateName);
			NamedProperties widgetAttributes = UISystem::ExtractWidgetAttributesFromStateDefinition(stateDefinition);
			
			AddTarget(widgetName);
			AddProperty(state, widgetAttributes);
		}
	}
}

void WidgetStyle::AddTarget(const std::string& widgetName)
{
	m_targetWidgets.insert(widgetName);
}

void WidgetStyle::AddGeneralProperty(const NamedProperties& prop)
{
	AddProperty(UI_STATE_ALL, prop);

}

void WidgetStyle::AddProperty(UIState st, const NamedProperties& prop)
{
	State state;
	state.type = STATE_NONE;
	state.state1 = st;

	GetProperties()[state] = prop;
}

void WidgetStyle::AddEntryProperty(UIState st, const NamedProperties& prop)
{
	State state;
	state.type = STATE_ENTER;
	state.state1 = st;

	GetProperties()[state] = prop;
	
}

void WidgetStyle::AddExitProperty(UIState st, const NamedProperties& prop)
{
	State state;
	state.type = STATE_EXIT;
	state.state1 = st;

	GetProperties()[state] = prop;
}

void WidgetStyle::AddTransitionProperty(UIState start, UIState end, const NamedProperties& prop)
{
	State st;
	st.state1 = start;
	st.state2 = end;
	st.type = STATE_TRANSITION;

	GetProperties()[st] = prop;
}


bool WidgetStyle::Applies(const std::string& widgetName) const
{
	return m_targetWidgets.count(widgetName) > 0 || m_targetWidgets.empty();
}

WidgetStyle::~WidgetStyle()
{
}
