#include "WidgetStyle.hpp"
#include "UISystem.hpp"


WidgetStyle::WidgetStyle()
{
}

NamedProperties WidgetStyle::ExtractWidgetAttributesFromStateDefinition(const TiXmlNode* stateDefinition) {
	NamedProperties widgetAttributes;

	const char* stateName = stateDefinition->ToElement()->Value();
	const TiXmlNode* bgcolor = stateDefinition->FirstChild("BackgroundColor");

	const TiXmlNode* bordersize = stateDefinition->FirstChild("BorderSize");
	const TiXmlNode* borderColor = stateDefinition->FirstChild("BorderColor");
	const TiXmlNode* size = stateDefinition->FirstChild("Size");
	const TiXmlNode* offset = stateDefinition->FirstChild("Offset");
	const TiXmlNode* opacity = stateDefinition->FirstChild("Opacity");


	if (bgcolor) {
		const TiXmlElement* bgColorElement = bgcolor->ToElement();
		const char* R = bgColorElement->Attribute("R");
		const char* G = bgColorElement->Attribute("G");
		const char* B = bgColorElement->Attribute("B");
		const char* A = bgColorElement->Attribute("A");

		bool isAnim = bgcolor->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<RGBA> colorSeq;
			const char* durationSeconds = bgColorElement->Attribute("duration");
			colorSeq.SetDuration(atof(durationSeconds));

			for (const TiXmlNode* animationDefinition = bgcolor->FirstChild("KeyFrame"); animationDefinition; animationDefinition = animationDefinition->NextSibling())
			{
				ParseColorAnimation(colorSeq, animationDefinition);
			}

			widgetAttributes.Set("color", colorSeq);
		}
		else {

			if (!A)
				A = "255";

			char Rc = (char)atoi(R);
			char Gc = (char)atoi(G);
			char Bc = (char)atoi(B);
			char Ac = (char)atoi(A);

			KeyFrameAnimation<RGBA> colorSeq;
			colorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), 0.f);
			widgetAttributes.Set("color", colorSeq);
		}
	}

	if (size) {

		const TiXmlElement* sizeElement = size->ToElement();

		const char* sizeX = sizeElement->Attribute("X");
		const char* sizeY = sizeElement->Attribute("Y");

		float X = atof(sizeX);
		float Y = atof(sizeY);

		KeyFrameAnimation<Vec2> sizeSeq;
		sizeSeq.AddAnimationFrameAtParameter(Vec2(X, Y), 0.f);

		widgetAttributes.Set("size", sizeSeq);
	}

	if (offset) {
		const TiXmlElement* offsetElement = offset->ToElement();
		const char* offsetX = offsetElement->Attribute("X");
		const char* offsetY = offsetElement->Attribute("Y");

		float X = atof(offsetX);
		float Y = atof(offsetY);

		KeyFrameAnimation<Vec2> offsetSeq;
		offsetSeq.AddAnimationFrameAtParameter(Vec2(X, Y), 0.f);

		widgetAttributes.Set("offset", offsetSeq);

	}

	if (opacity) {
		KeyFrameAnimation <float> opacitySeq;
		float opacityVal = atof(opacity->ToElement()->Attribute("opacity"));
		opacitySeq.AddAnimationFrameAtParameter(0.f, opacityVal);

		widgetAttributes.Set("opacity", opacitySeq);
	}

	return widgetAttributes;
}


WidgetStyle::WidgetStyle(const TiXmlNode* node)
{
		const char* widgetName = node->ToElement()->Value();

		//Iterate over each state definition for the widget type
		for (const TiXmlNode* stateDefinition = node->FirstChild(); node; node = node->NextSibling())
		{
			std::string stateName = stateDefinition->ToElement()->Value();
			UIState state = WidgetBase::GetStateForName(stateName);
			NamedProperties widgetAttributes = ExtractWidgetAttributesFromStateDefinition(stateDefinition);
			
			AddTarget(widgetName);
			AddProperty(state, widgetAttributes);
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

void WidgetStyle::ParseColorAnimation(KeyFrameAnimation<RGBA>& colorSeq, const TiXmlNode* animationDefinition)
{
	{
		const TiXmlElement* animationElement = animationDefinition->ToElement();
		const char* R = animationElement->Attribute("R");
		const char* G = animationElement->Attribute("G");
		const char* B = animationElement->Attribute("B");
		const char* A = animationElement->Attribute("A");
		const char* time = animationElement->Attribute("time");

		char Rc = (char)atoi(R);
		char Gc = (char)atoi(G);
		char Bc = (char)atoi(B);
		char Ac = (char)atoi(A);

		if (!A)
			A = "255";

		colorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), atof(time));
	}
}
