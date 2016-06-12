#include "WidgetStyle.hpp"
#include "UISystem.hpp"
#include <algorithm>
#include <iterator>

WidgetStyle::WidgetStyle()
{
}

WidgetStyle::~WidgetStyle()
{
}

NamedProperties WidgetStyle::ExtractWidgetAttributesFromStateDefinition(const TiXmlNode* stateDefinition) {
	NamedProperties widgetAttributes;

	const char* stateName = stateDefinition->ToElement()->Value();
	const TiXmlNode* bgcolor = stateDefinition->FirstChild("BackgroundColor");

	const TiXmlNode* borderSize = stateDefinition->FirstChild("BorderSize");
	const TiXmlNode* borderColor = stateDefinition->FirstChild("BorderColor");
	const TiXmlNode* size = stateDefinition->FirstChild("Size");
	const TiXmlNode* offset = stateDefinition->FirstChild("Offset");
	const TiXmlNode* opacity = stateDefinition->FirstChild("Opacity");
	const TiXmlNode* textColor = stateDefinition->FirstChild("TextColor");
	const TiXmlNode* fontSize = stateDefinition->FirstChild("FontSize");
	const TiXmlNode* textOpacity = stateDefinition->FirstChild("TextOpacity");


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
			const char* isLooping = bgColorElement->Attribute("loop");
			if (strcmp(isLooping, "true") == 0)
				colorSeq.SetWrapMode(B_LOOP);

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
		float opacityVal = atof(opacity->ToElement()->Attribute("value"));
		opacitySeq.AddAnimationFrameAtParameter(opacityVal, 0.f);

		widgetAttributes.Set("opacity", opacitySeq);
	}

	if (borderSize) {
		KeyFrameAnimation <float> borderSizeSeq;
		float borderSizeVal = atof(borderSize->ToElement()->Attribute("value"));
		borderSizeSeq.AddAnimationFrameAtParameter(borderSizeVal, 0.f);

		widgetAttributes.Set("border size", borderSizeSeq);
	}

	if (borderColor) {
		KeyFrameAnimation <RGBA> borderColorSeq;

		const TiXmlElement* bgColorElement = borderColor->ToElement();
		const char* R = bgColorElement->Attribute("R");
		const char* G = bgColorElement->Attribute("G");
		const char* B = bgColorElement->Attribute("B");
		const char* A = bgColorElement->Attribute("A");

		char Rc = (char)atoi(R);
		char Gc = (char)atoi(G);
		char Bc = (char)atoi(B);
		char Ac;

		if (!A)
			Ac = 255;
		else
			Ac = (char)atoi(A);

		borderColorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), 0.f);

		widgetAttributes.Set("border color", borderColorSeq);
	}

	if (textColor) {
		KeyFrameAnimation <RGBA> textColorSeq;

		const TiXmlElement* bgColorElement = textColor->ToElement();
		const char* R = bgColorElement->Attribute("R");
		const char* G = bgColorElement->Attribute("G");
		const char* B = bgColorElement->Attribute("B");
		const char* A = bgColorElement->Attribute("A");

		char Rc = (char)atoi(R);
		char Gc = (char)atoi(G);
		char Bc = (char)atoi(B);
		char Ac;

		if (!A)
			Ac = 255;
		else
			Ac = (char)atoi(A);

		textColorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), 0.f);

		widgetAttributes.Set("text color", textColorSeq);
	}

	if (fontSize) {
		KeyFrameAnimation <float> fontSizeSeq;

		const TiXmlElement* fontSizeElement = fontSize->ToElement();
		float fontSize = atof(fontSizeElement->Attribute("value"));

		fontSizeSeq.AddAnimationFrameAtParameter(fontSize, 0.f);
		widgetAttributes.Set("text scale", fontSizeSeq);
	}

	if (textOpacity) {
		KeyFrameAnimation <float> textOpacityAnim;

		const TiXmlElement* textOpacityElement = textOpacity->ToElement();
		float textOpacity = atof(textOpacityElement->Attribute("value"));

		textOpacityAnim.AddAnimationFrameAtParameter(textOpacity, 0.f);
		widgetAttributes.Set("text opacity", textOpacityAnim);
	}

	return widgetAttributes;
}


WidgetStyle::WidgetStyle(const TiXmlNode* node)
{
	std::vector<std::string> statesInXML;
	std::vector<std::string> allStates;
	std::vector<std::string> diff;
	for (UIState state = UI_STATE_DEFAULT; state < (NUM_UI_STATES - 1); state = (UIState)(state + 1)) {
		allStates.push_back(WidgetBase::GetNameForState(state));
	}
	std::sort(allStates.begin(), allStates.end());
	const char* widgetName = node->ToElement()->Value();
	AddTarget(widgetName);

	//Iterate over each state definition for the widget type

	const TiXmlNode* allDef = node->FirstChild("All");
	NamedProperties allAttributes = ExtractWidgetAttributesFromStateDefinition(allDef);
	auto allAttributesMap = allAttributes.GetPropertyMap();
	for (const TiXmlNode* stateDefinition = node->FirstChild(); stateDefinition; stateDefinition = stateDefinition->NextSibling())
	{
		std::string stateName = stateDefinition->ToElement()->Value();
		statesInXML.push_back(stateName);
		if (stateName != "All")
		{
			UIState state = WidgetBase::GetStateForName(stateName);
			NamedProperties widgetAttributes = ExtractWidgetAttributesFromStateDefinition(stateDefinition);
			widgetAttributes.GetPropertyMap().insert(allAttributesMap.begin(), allAttributesMap.end());
			AddProperty(state, widgetAttributes);


		}
	}

	std::sort(statesInXML.begin(), statesInXML.end());
	std::set_difference(allStates.begin(), allStates.end(), statesInXML.begin(), statesInXML.end(), std::inserter(diff, diff.begin()));

	for (auto it = diff.begin(); it != diff.end(); ++it)
	{
		UIState state = WidgetBase::GetStateForName(*it);
		AddProperty(state, allAttributes);
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
