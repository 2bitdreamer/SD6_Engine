#pragma once
#include "WidgetBase.hpp"
#include "Libraries\tinyxml.h"
#include <set>
#include <map>
#include <xfunctional>

enum StateType
{ 
	STATE_ENTER, STATE_EXIT, STATE_TRANSITION, STATE_NONE
};

struct State {
	StateType type;
	UIState state1;
	UIState state2;
};

class WidgetStyle {
public:
	/* Constructors */
	WidgetStyle();

	/* 7.a */
	/* Dispatches to [C++ Definition Methods] */
	WidgetStyle(const TiXmlNode *);

	/* 7.b */
	/* C++ Definition Methods */
	/* 7.c */
	void AddTarget(const std::string& widgetName);
	/* 7.d */
	void AddGeneralProperty(const NamedProperties& prop);
	/* 7.e */
	void AddProperty(UIState st, const NamedProperties& prop);
	/* 7.f */
	void AddEntryProperty(UIState st, const NamedProperties& prop);
	void AddExitProperty(UIState st, const NamedProperties& prop);
	/* 7.g */
	void AddTransitionProperty(UIState start, UIState end, const NamedProperties& prop);

	/* Getters */
	bool Applies(const std::string& widgetName) const;

	std::map<WidgetStyle::State, NamedProperties, std::greater<WidgetStyle::State>> GetProperties() const { return m_properties; }
	// ...

private:
	std::set<std::string> m_targetWidgets;

	/*
	General properties come before specific properties
	*/
	std::map<State, NamedProperties, std::greater<State>> m_properties;

};

