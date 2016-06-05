#pragma once
#include <map>
#include <typeinfo.h>
#include <string>
#include "Assert.hpp"

class TypedPropertyBase {
public:
	virtual bool IsOfType(const type_info& compareType) = 0;
	virtual TypedPropertyBase* Clone() = 0;
};

enum PropertyGetResult {
	RESULT_SUCCESS,
	RESULT_FAILED_NOT_FOUND,
	RESULT_FAILED_WRONG_TYPE,
	RESULT_FAILED_NO_PROPERTIES
};

class NamedProperties
{
public:
	NamedProperties();
	~NamedProperties();

	template<typename T_PropertyType1, typename T_PropertyType2, typename T_PropertyType3>
	NamedProperties(const std::string& propertyName1, const T_PropertyType1& propVal1, const std::string& propertyName2, const T_PropertyType2& propVal2, const std::string& propertyName3, const T_PropertyType3& propVal3);

	template<typename T_PropertyType1, typename T_PropertyType2>
	NamedProperties(const std::string& propertyName1, const T_PropertyType1& propVal1, const std::string& propertyName2, const T_PropertyType2& propVal2);

	template<typename T_PropertyType1>
	NamedProperties(const std::string& propertyName1, const T_PropertyType1& propVal1);

	template<typename T_PropertyType>
	void Set(const std::string& propertyName, const T_PropertyType& propVal);

	template<typename T_PropertyType>
	PropertyGetResult Get(const std::string propertyName, T_PropertyType& out_propertyValue) const;
	template <typename T> T& Get(std::string const & name);
	NamedProperties(const NamedProperties& np);


private:
	std::map<std::string, TypedPropertyBase*> m_properties;

};


template<typename T_PropertyType>
class TypedProperty : public TypedPropertyBase {
public:

	TypedProperty(const T_PropertyType& propertyValue);
	virtual TypedPropertyBase* Clone() {
		TypedProperty<T_PropertyType>* clone = new TypedProperty<T_PropertyType>(m_propertyValue);
		clone->m_propertyValue = m_propertyValue;
		return clone;
	}

	virtual bool IsOfType(const type_info& compareType) {
		const type_info& myType = typeid(m_propertyValue);
		return (myType == compareType);
	}

	T_PropertyType m_propertyValue;
};

template<typename T_PropertyType>
TypedProperty<T_PropertyType>::TypedProperty(const T_PropertyType& propVal) :
	m_propertyValue(propVal)
{}

template<typename T_PropertyType1>
NamedProperties::NamedProperties(const std::string& propertyName1, const T_PropertyType1& propVal1)
{
	Set(propertyName1, propVal1);
}

template<typename T_PropertyType1, typename T_PropertyType2>
NamedProperties::NamedProperties(const std::string& propertyName1, const T_PropertyType1& propVal1, const std::string& propertyName2, const T_PropertyType2& propVal2)
{
	Set(propertyName1, propVal1);
	Set(propertyName2, propVal2);
}

template<typename T_PropertyType1, typename T_PropertyType2, typename T_PropertyType3>
NamedProperties::NamedProperties(const std::string& propertyName1, const T_PropertyType1& propVal1, const std::string& propertyName2, const T_PropertyType2& propVal2, const std::string& propertyName3, const T_PropertyType3& propVal3)
{
	Set(propertyName1, propVal1);
	Set(propertyName2, propVal2);
	Set(propertyName3, propVal3);
}

template<typename T>
void NamedProperties::Set(const std::string& propertyName, const T& propertyValue)
{
	auto found = m_properties.find(propertyName);

	if (found != m_properties.end()) {
		TypedPropertyBase* prop = found->second;

		TypedPropertyBase* propertyBase = found->second;
		if (propertyBase->IsOfType(typeid(T))) {
			TypedProperty<T>* theTypedProperty = reinterpret_cast<TypedProperty<T>*>(propertyBase);
			theTypedProperty->m_propertyValue = propertyValue;
			return;
		}
		else {
			delete prop;
		}
	}

	TypedPropertyBase* newProperty = new TypedProperty<T>(propertyValue);
	m_properties[propertyName] = newProperty;
}

template <typename T_PropertyType>
T_PropertyType& NamedProperties::Get(std::string const & propertyName) {

	bool error = false;
	if (m_properties.empty())
		error = true;

	auto found = m_properties.find(propertyName);
	if (found == m_properties.end())
		error = true;

	TypedPropertyBase* baseProperty = found->second;
	if (!baseProperty->IsOfType(typeid(T_PropertyType)))
		error = true;

	FATAL_ASSERT(!error);

	TypedProperty<T_PropertyType>* theTypedProperty = reinterpret_cast<TypedProperty<T_PropertyType>*>(baseProperty);
	T_PropertyType& propertyValue = theTypedProperty->m_propertyValue;
	return propertyValue;
}

template<typename T_PropertyType>
PropertyGetResult NamedProperties::Get(const std::string propertyName, T_PropertyType& out_propertyValue) const
{
	if (m_properties.empty())
		return RESULT_FAILED_NO_PROPERTIES;

	auto found = m_properties.find(propertyName);
	if (found == m_properties.end())
		return RESULT_FAILED_NOT_FOUND;

	TypedPropertyBase* baseProperty = found->second;
	if (!baseProperty->IsOfType(typeid(T_PropertyType)))
		return RESULT_FAILED_WRONG_TYPE;

	TypedProperty<T_PropertyType>* theTypedProperty = reinterpret_cast<TypedProperty<T_PropertyType>*>(baseProperty);
	out_propertyValue = theTypedProperty->m_propertyValue;
	return RESULT_SUCCESS;
}


