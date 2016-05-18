#include "NamedProperties.hpp"



NamedProperties::NamedProperties()
{
}


NamedProperties::NamedProperties(const NamedProperties& np)
{
	for (auto it = np.m_properties.begin(); it != np.m_properties.end(); ++it) {
		TypedPropertyBase* current = it->second;
		TypedPropertyBase* cloneProperty = current->Clone();
		m_properties.insert(std::make_pair(it->first, cloneProperty));
	}
}

NamedProperties::~NamedProperties()
{
}
