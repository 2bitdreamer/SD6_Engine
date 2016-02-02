#include "StringTable.hpp"
#include <assert.h>
#include <limits>

std::unordered_map<std::string, int> StringTable::s_stringToIDTable;
std::unordered_map<int, std::string> StringTable::s_IDtoStringTable;

int StringTable::s_nextStringTableID;

StringTable::StringTable()
{
}


StringTable::~StringTable()
{
}

unsigned int StringTable::GetIDFromString(const std::string& name)
{
	auto iter = s_stringToIDTable.find(name);
	if (iter != s_stringToIDTable.end())
		return iter->second;
	else {
		s_stringToIDTable[name] = s_nextStringTableID;
		s_IDtoStringTable[s_nextStringTableID] = name;

		s_nextStringTableID++;
		return std::numeric_limits<unsigned int>::max();
	}
}

std::string StringTable::GetStringFromID(unsigned int stringID)
{
	auto iter = s_IDtoStringTable.find(stringID);
	assert(iter != s_IDtoStringTable.end());
	return iter->second;
}
