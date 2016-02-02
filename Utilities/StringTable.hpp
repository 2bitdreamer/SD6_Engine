#pragma once
#include <string>
#include <unordered_map>

class StringTable
{
public:
	StringTable();
	~StringTable();

	static unsigned int GetIDFromString(const std::string& name);
	static std::string GetStringFromID(unsigned int stringID);

protected:
	static std::unordered_map<std::string, int> s_stringToIDTable;
	static std::unordered_map<int, std::string> s_IDtoStringTable;

	static int s_nextStringTableID;
};

