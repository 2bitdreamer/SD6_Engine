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


KeyFrameAnimation<RGBA> WidgetStyle::SetupColorAnimation(const TiXmlElement* colorElement) {
	KeyFrameAnimation<RGBA> colorSeq;

	const char* durationSeconds = colorElement->Attribute("duration");
	const char* isLooping = colorElement->Attribute("loop");
	if (strcmp(isLooping, "true") == 0)
		colorSeq.SetWrapMode(B_LOOP);

	colorSeq.SetDuration((float)atof(durationSeconds));

	for (const TiXmlNode* animationDefinition = colorElement->FirstChild("KeyFrame"); animationDefinition; animationDefinition = animationDefinition->NextSibling())
	{
		ParseColorAnimation(colorSeq, animationDefinition);
	}

	return colorSeq;
}



KeyFrameAnimation<float> WidgetStyle::SetupFloatAnimation(const TiXmlElement* colorElement) {
	KeyFrameAnimation<float> floatSeq;

	const char* durationSeconds = colorElement->Attribute("duration");
	const char* isLooping = colorElement->Attribute("loop");
	if (strcmp(isLooping, "true") == 0)
		floatSeq.SetWrapMode(B_LOOP);

	floatSeq.SetDuration((float)atof(durationSeconds));

	for (const TiXmlNode* animationDefinition = colorElement->FirstChild("KeyFrame"); animationDefinition; animationDefinition = animationDefinition->NextSibling())
	{
		ParseFloatAnimation(floatSeq, animationDefinition);
	}

	return floatSeq;
}

KeyFrameAnimation<Vec2> WidgetStyle::SetupVec2Animation(const TiXmlElement* vec2Element) {
	KeyFrameAnimation<Vec2> vec2Seq;

	const char* durationSeconds = vec2Element->Attribute("duration");
	const char* isLooping = vec2Element->Attribute("loop");
	if (strcmp(isLooping, "true") == 0)
		vec2Seq.SetWrapMode(B_LOOP);

	vec2Seq.SetDuration((float)atof(durationSeconds));

	for (const TiXmlNode* animationDefinition = vec2Element->FirstChild("KeyFrame"); animationDefinition; animationDefinition = animationDefinition->NextSibling())
	{
		ParseVec2Animation(vec2Seq, animationDefinition);
	}

	return vec2Seq;
}


void WidgetStyle::ParseFloatAnimation(KeyFrameAnimation<float>& floatSeq, const TiXmlNode* animationDefinition)
{
		const TiXmlElement* animationElement = animationDefinition->ToElement();
		const char* valElement = animationElement->Attribute("value");
		const char* time = animationElement->Attribute("time");

		float val = (float)atof(valElement);
		floatSeq.AddAnimationFrameAtParameter(val, (float)atof(time));
}



NamedProperties WidgetStyle::ExtractWidgetAttributesFromStateDefinition(const TiXmlNode* stateDefinition) {
	NamedProperties widgetAttributes;

	//const char* stateName = stateDefinition->ToElement()->Value();
	const char* onExit = stateDefinition->ToElement()->Attribute("OnExit");
	const char* onEnter = stateDefinition->ToElement()->Attribute("OnEnter");

	if (onEnter) {
		KeyFrameAnimation<std::vector<std::string>> enterSeq;
		enterSeq.AddAnimationFrameAtParameter(SplitString(onEnter, ","), 0.f);

		widgetAttributes.Set("enter event", enterSeq);
	}

	if (onExit) {
		KeyFrameAnimation<std::vector<std::string>> exitSeq;
		exitSeq.AddAnimationFrameAtParameter(SplitString(onExit, ","), 0.f);

		widgetAttributes.Set("exit event", exitSeq);
	}

	const TiXmlNode* bgcolor = stateDefinition->FirstChild("BackgroundColor");

	const TiXmlNode* borderSize = stateDefinition->FirstChild("BorderSize");
	const TiXmlNode* borderColor = stateDefinition->FirstChild("BorderColor");
	const TiXmlNode* size = stateDefinition->FirstChild("Size");
	const TiXmlNode* offset = stateDefinition->FirstChild("Offset");
	const TiXmlNode* opacity = stateDefinition->FirstChild("Opacity");
	const TiXmlNode* textColor = stateDefinition->FirstChild("TextColor");
	const TiXmlNode* fontSize = stateDefinition->FirstChild("FontSize");
	const TiXmlNode* textOpacity = stateDefinition->FirstChild("TextOpacity");
	const TiXmlNode* textContents = stateDefinition->FirstChild("TextContents");
	const TiXmlNode* innerColor = stateDefinition->FirstChild("InnerColor");



	if (bgcolor) {
		const TiXmlElement* bgColorElement = bgcolor->ToElement();
		const char* R = bgColorElement->Attribute("R");
		const char* G = bgColorElement->Attribute("G");
		const char* B = bgColorElement->Attribute("B");
		const char* A = bgColorElement->Attribute("A");

		bool isAnim = bgcolor->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<RGBA> colorSeq = SetupColorAnimation(bgColorElement);
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
		bool isAnim = size->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<Vec2> sizeSeq = SetupVec2Animation(sizeElement);
			widgetAttributes.Set("size", sizeSeq);
		}
		else {
			const char* sizeX = sizeElement->Attribute("X");
			const char* sizeY = sizeElement->Attribute("Y");

			float X = (float)atof(sizeX);
			float Y = (float)atof(sizeY);

			KeyFrameAnimation<Vec2> sizeSeq;
			sizeSeq.AddAnimationFrameAtParameter(Vec2(X, Y), 0.f);

			widgetAttributes.Set("size", sizeSeq);
		}
	}

	if (offset) {
		const TiXmlElement* offsetElement = offset->ToElement();

		bool isAnim = offset->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<Vec2> offsetSeq = SetupVec2Animation(offsetElement);
			widgetAttributes.Set("offset", offsetSeq);
		}
		else {
			const char* offsetX = offsetElement->Attribute("X");
			const char* offsetY = offsetElement->Attribute("Y");

			float X = (float)atof(offsetX);
			float Y = (float)atof(offsetY);

			KeyFrameAnimation<Vec2> offsetSeq;
			offsetSeq.AddAnimationFrameAtParameter(Vec2(X, Y), 0.f);

			widgetAttributes.Set("offset", offsetSeq);
		}
	}

	if (opacity) {
		bool isAnim = opacity->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<float> opacitySeq = SetupFloatAnimation(opacity->ToElement());
			widgetAttributes.Set("opacity", opacitySeq);
		}
		else {
			KeyFrameAnimation <float> opacitySeq;
			float opacityVal = (float)atof(opacity->ToElement()->Attribute("value"));
			opacitySeq.AddAnimationFrameAtParameter(opacityVal, 0.f);

			widgetAttributes.Set("opacity", opacitySeq);
		}
	}

	if (borderSize) {

		bool isAnim = borderSize->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<float> borderSizeSeq = SetupFloatAnimation(borderSize->ToElement());
			widgetAttributes.Set("border size", borderSizeSeq);
		}
		else {
			KeyFrameAnimation <float> borderSizeSeq;
			float borderSizeVal = (float)atof(borderSize->ToElement()->Attribute("value"));
			borderSizeSeq.AddAnimationFrameAtParameter(borderSizeVal, 0.f);

			widgetAttributes.Set("border size", borderSizeSeq);
		}
	}

	if (borderColor) {

		bool isAnim = borderColor->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<RGBA> colorSeq = SetupColorAnimation(borderColor->ToElement());
			widgetAttributes.Set("border color", colorSeq);
		}
		else {
			KeyFrameAnimation <RGBA> borderColorSeq;

			const TiXmlElement* borderColorElement = borderColor->ToElement();
			const char* R = borderColorElement->Attribute("R");
			const char* G = borderColorElement->Attribute("G");
			const char* B = borderColorElement->Attribute("B");
			const char* A = borderColorElement->Attribute("A");

			unsigned char Rc = (unsigned char)atoi(R);
			unsigned char Gc = (unsigned char)atoi(G);
			unsigned char Bc = (unsigned char)atoi(B);
			unsigned char Ac = '0';

			if (!A)
				Ac = 255;
			else
				Ac = (char)atoi(A);

			borderColorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), 0.f);

			widgetAttributes.Set("border color", borderColorSeq);
		}
	}

	if (textColor) {

		bool isAnim = textColor->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<RGBA> colorSeq = SetupColorAnimation(textColor->ToElement());
			widgetAttributes.Set("text color", colorSeq);
		}
		else {
			KeyFrameAnimation <RGBA> textColorSeq;

			const TiXmlElement* bgColorElement = textColor->ToElement();
			const char* R = bgColorElement->Attribute("R");
			const char* G = bgColorElement->Attribute("G");
			const char* B = bgColorElement->Attribute("B");
			const char* A = bgColorElement->Attribute("A");

			unsigned char Rc = (unsigned char)atoi(R);
			unsigned char Gc = (unsigned char)atoi(G);
			unsigned char Bc = (unsigned char)atoi(B);
			unsigned char Ac = '0';

			if (!A)
				Ac = 255;
			else
				Ac = (char)atoi(A);

			textColorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), 0.f);

			widgetAttributes.Set("text color", textColorSeq);
		}
	}

	if (fontSize) {

		bool isAnim = fontSize->FirstChild("KeyFrame") != nullptr;
		const TiXmlElement* fontSizeElement = fontSize->ToElement();

		if (isAnim) {
			KeyFrameAnimation<float> fontSizeSeq = SetupFloatAnimation(fontSizeElement);
			widgetAttributes.Set("font size", fontSizeSeq);
		}
		else {
			KeyFrameAnimation <float> fontSizeSeq;
			float fontS = (float)atof(fontSizeElement->Attribute("value"));

			fontSizeSeq.AddAnimationFrameAtParameter(fontS, 0.f);
			widgetAttributes.Set("text scale", fontSizeSeq);
		}
	}

	if (textOpacity) {

		bool isAnim = textOpacity->FirstChild("KeyFrame") != nullptr;
		const TiXmlElement* textOpacityElement = textOpacity->ToElement();

		if (isAnim) {
			KeyFrameAnimation<float> textOpacitySeq = SetupFloatAnimation(textOpacityElement);
			widgetAttributes.Set("text opacity", textOpacitySeq);
		}
		else {
			KeyFrameAnimation <float> textOpacityAnim;
			float textOpacityVal = (float)atof(textOpacityElement->Attribute("value"));

			textOpacityAnim.AddAnimationFrameAtParameter(textOpacityVal, 0.f);
			widgetAttributes.Set("text opacity", textOpacityAnim);
		}
	}

	if (textContents) {
		KeyFrameAnimation <std::string> textcontentsAnim;

		const TiXmlElement* textContentsElement = textContents->ToElement();
		const std::string textContentsStr = textContentsElement->Attribute("value");

		textcontentsAnim.AddAnimationFrameAtParameter(textContentsStr, 0.f);
		widgetAttributes.Set("text", textcontentsAnim);
	}

	if (innerColor) {
		bool isAnim = innerColor->FirstChild("KeyFrame") != nullptr;

		if (isAnim) {
			KeyFrameAnimation<RGBA> innerColorSeq = SetupColorAnimation(innerColor->ToElement());
			widgetAttributes.Set("inner color", innerColorSeq);
		}
		else {
			KeyFrameAnimation <RGBA> innerColorSeq;

			const TiXmlElement* innerColorElement = innerColor->ToElement();
			const char* R = innerColorElement->Attribute("R");
			const char* G = innerColorElement->Attribute("G");
			const char* B = innerColorElement->Attribute("B");
			const char* A = innerColorElement->Attribute("A");

			unsigned char Rc = (unsigned char)atoi(R);
			unsigned char Gc = (unsigned char)atoi(G);
			unsigned char Bc = (unsigned char)atoi(B);
			unsigned char Ac = '0';

			if (!A)
				Ac = 255;
			else
				Ac = (char)atoi(A);

			innerColorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), 0.f);

			widgetAttributes.Set("inner color", innerColorSeq);
		}
	}

	return widgetAttributes;
}


WidgetStyle::WidgetStyle(const TiXmlNode* node)
{
	const TiXmlElement* asElement = node->ToElement();
	if (asElement) {

	}

	std::vector<std::string> statesInXML;
	const char* widgetName = node->ToElement()->Value();
	AddTarget(widgetName);

	for (const TiXmlNode* stateDefinition = node->FirstChild(); stateDefinition; stateDefinition = stateDefinition->NextSibling())
	{
		if (!stateDefinition->FirstChild())
			continue;

		std::string stateName = stateDefinition->ToElement()->Value();
		statesInXML.push_back(stateName);
		NamedProperties widgetAttributes = ExtractWidgetAttributesFromStateDefinition(stateDefinition);
		if (stateName != "All")
		{
			UIState state = WidgetBase::GetStateForName(stateName);
			AddProperty(state, widgetAttributes);
		}
		else
		{
			AddGeneralProperty(widgetAttributes);
		}
	}


	/*
	if (node) {
	const char* updateEventToFire = node->ToElement()->Attribute("OnUpdate");
	const char* renderEventToFire = node->ToElement()->Attribute("OnRender");
	const char* clickEventToFire = node->ToElement()->Attribute("OnClick");
	NamedProperties eventNP;
	if (updateEventToFire) {
	eventNP.Set("update event", std::string(updateEventToFire));
	}
	if (renderEventToFire) {
	eventNP.Set("render event", std::string(renderEventToFire));
	}
	if (clickEventToFire) {
	eventNP.Set("click event", std::string(clickEventToFire));
	}

	if (!eventNP.GetPropertyMap().empty())
	AddGeneralProperty(eventNP);
	}
	*/

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

		colorSeq.AddAnimationFrameAtParameter(RGBA(Rc, Gc, Bc, Ac), (float)atof(time));
	}
}

void WidgetStyle::ParseVec2Animation(KeyFrameAnimation<Vec2>& vec2Seq, const TiXmlNode* animationDefinition)
{
	const TiXmlElement* animationElement = animationDefinition->ToElement();
	const char* x = animationElement->Attribute("X");
	const char* y = animationElement->Attribute("Y");
	const char* time = animationElement->Attribute("time");

	vec2Seq.AddAnimationFrameAtParameter(Vec2((float)atof(x), (float)atof(y)),(float)atof(time));
}

void WidgetStyle::MergeStyle(WidgetStyle* tempStyle)
{
	KeyFrameAnimation<Vec2> offset;
	KeyFrameAnimation<Vec2> size;
	KeyFrameAnimation<RGBA> color;
	KeyFrameAnimation<RGBA> edgeColor;
	KeyFrameAnimation<RGBA> innerColor;
	KeyFrameAnimation<RGBA> textColor;

	KeyFrameAnimation<float> borderSize;
	KeyFrameAnimation<float> opacity;
	KeyFrameAnimation<float> duration;
	
	KeyFrameAnimation<float> textScale;
	KeyFrameAnimation<float> textOpacity;
	KeyFrameAnimation<std::string> textContents;
	KeyFrameAnimation<std::vector<std::string>> updateEvent;
	KeyFrameAnimation<std::vector<std::string>> renderEvent;
	KeyFrameAnimation<std::vector<std::string>> enterEvent;
	KeyFrameAnimation<std::vector<std::string>> exitEvent;

	std::map<State, NamedProperties>& oldMap = m_properties;
	std::map<State, NamedProperties>& newMap = tempStyle->m_properties;

	for (auto& it : newMap)
	{
		State curState = it.first;
		NamedProperties& curNPNew = it.second;

		PropertyGetResult ofr = curNPNew.Get("offset", offset);
		PropertyGetResult sr = curNPNew.Get("size", size);
		PropertyGetResult cr = curNPNew.Get("color", color);
		PropertyGetResult opr = curNPNew.Get("opacity", opacity);
		PropertyGetResult ec = curNPNew.Get("border color", edgeColor);
		PropertyGetResult bs = curNPNew.Get("border size", borderSize);
		PropertyGetResult tc = curNPNew.Get("text color", textColor);
		PropertyGetResult ts = curNPNew.Get("text scale", textScale);
		PropertyGetResult to = curNPNew.Get("text opacity", textOpacity);
		PropertyGetResult tcon = curNPNew.Get("text", textContents);
		PropertyGetResult icol = curNPNew.Get("inner color", innerColor);
		PropertyGetResult updEv = curNPNew.Get("update event", updateEvent);
		PropertyGetResult renEv = curNPNew.Get("render event", renderEvent);
		PropertyGetResult enEv = curNPNew.Get("enter event", enterEvent);
		PropertyGetResult exEv = curNPNew.Get("exit event", exitEvent);
		PropertyGetResult durEv = curNPNew.Get("duration", duration);

		if (ofr == RESULT_SUCCESS)
			oldMap[curState].Set("offset", offset);

		if (sr == RESULT_SUCCESS)
			oldMap[curState].Set("size", size);

		if (cr == RESULT_SUCCESS)
			oldMap[curState].Set("color", color);

		if (opr == RESULT_SUCCESS)
			oldMap[curState].Set("opacity", opacity);

		if (ec == RESULT_SUCCESS)
			oldMap[curState].Set("border color", edgeColor);

		if (bs == RESULT_SUCCESS)
			oldMap[curState].Set("border size", borderSize);

		if (tc == RESULT_SUCCESS)
			oldMap[curState].Set("text color", textColor);

		if (ts == RESULT_SUCCESS)
			oldMap[curState].Set("text scale", textScale);

		if (to == RESULT_SUCCESS)
			oldMap[curState].Set("text opacity", textOpacity);

		if (tcon == RESULT_SUCCESS)
			oldMap[curState].Set("text", textContents);

		if (icol == RESULT_SUCCESS)
			oldMap[curState].Set("inner color", innerColor);

		if (updEv == RESULT_SUCCESS)
			oldMap[curState].Set("update event", updateEvent);

		if (renEv == RESULT_SUCCESS)
			oldMap[curState].Set("render event", renderEvent);

		if (enEv == RESULT_SUCCESS)
			oldMap[curState].Set("enter event", enterEvent);

		if (exEv == RESULT_SUCCESS)
			oldMap[curState].Set("exit event", exitEvent);

		if (durEv == RESULT_SUCCESS)
			oldMap[curState].Set("duration", durEv);
	}
}
