#pragma once
#if !defined(__COMMAND__)
#define __COMMAND__

#include <list>
#include <string>

class Command
{
public:
	std::string name;
	std::list<std::string> arguments;

	bool GetString(int idx, std::string &out) const
	{
		if ((idx < (int)arguments.size()) && (idx >= 0)) {
			auto iter = arguments.begin();
			while (idx > 0) {
				--idx;
				++iter;
			}

			out = *iter;
			return true;
		}
		else {
			return false;
		}
	}

	bool GetInt(int idx, int *out) const
	{
		std::string s;
		if (GetString(idx, s)) {
			*out = atoi(s.c_str());
			return true;
		}

		return false;
	}

	Command(char *n);
};

typedef void(*command_cb)(Command const*);
typedef std::list<Command*> CommandList;


CommandList ParseCommands(const std::string& commandLine);
void FreeCommands(CommandList commandList);


void RegisterCommand(std::string name, command_cb cb);

void ProcessCommands(CommandList commands);


#endif