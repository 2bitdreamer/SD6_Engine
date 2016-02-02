#include "Engine/Utilities/EngineCommon.hpp"
#include <windows.h>
#include "Command.hpp"

#include <map>

static char const COMMAND_INDICATOR = '-';
static char const QUOTE = '\"';
static char const ESCAPE = '\\';

static std::map<std::string, command_cb> gCommandLookup;

static char GetNextCharacter(char **stream)
{
	char *s = *stream;
	char c = *s;

	if (c == ESCAPE) {
		++s;
		switch (*(s)) {
		case 'n':
			c = '\n';
			break;
		case 'r':
			c = '\r';
			break;
		case '\\':
			c = '\\';
			break;
		case 't':
			c = '\t';
			break;
		case '\'':
			c = '\'';
			break;
		case '\"':
			c = '\"';
			break;
		case NULL:
			c = ESCAPE;
			--s; // go back one
			break;
		default:
			c = *s;
		}
	}

	*stream = s;
	return c;
}

static void GetNextString(char **out_stream, char **stream)
{
	char *s = *stream;
	char *out = *out_stream;

	// nothing to do, doesn't start with a quote;
	if (*s != QUOTE) {
		return;
	}

	++s;
	while ((*s != NULL) && (*s != QUOTE)) {
		char c = GetNextCharacter(&s);
		*out = c;
		++out;
		++s;
	}

	if ((*s) == QUOTE) {
		++s;
	}

	*out_stream = out;
	*stream = s;
}

static char* GetNextToken(char **stream)
{
	char *s = *stream;
	char *token = nullptr;
	// get up the first real character
	while (IsWhitespace(*s)) {
		++s;
	}

	if (*s != NULL) {
		token = s;
		char *out = s;

		if (*s == QUOTE) {
			GetNextString(&out, &s);
		}
		else {
			while (!IsWhitespace(*s) && (*s != NULL)) {
				char c = GetNextCharacter(&s);
				*out = c;
				++out;
				++s;
			}
		}

		// move past the possible "last position" we left on
		// since we're NULLing it out for our token
		if (*s != NULL) {
			++s;
		}
		*out = NULL;
	}

	*stream = s;
	return token;
}

static bool TokenIsNullOrEmpty(char const *token)
{
	return (token == nullptr) || (*token == NULL);
}

static bool TokenIsCommand(char const *token)
{
	if (token == nullptr) {
		return false;
	}

	return ((*token == COMMAND_INDICATOR)
		&& (*(token + 1) != NULL)
		&& (!IsWhitespace(*(token + 1))));
}



CommandList ParseCommands(const std::string& commandLine)
{
	size_t len = commandLine.size();
	char *stream = new char[len + 1];
	memcpy(stream, commandLine.c_str(), len);
	stream[len] = NULL;

	Command *command = nullptr;
	std::list<Command*> commands;

	char *s = stream;
	char *token = GetNextToken(&s);
	while (token != nullptr) {
		if (TokenIsCommand(token)) {
			if (command != nullptr) {
				commands.push_back(command);
			}

			command = new Command(token);
		}
		else if ((command != nullptr) && !TokenIsNullOrEmpty(token)) {
			command->arguments.push_back(token);
		}

		token = GetNextToken(&s);
	}

	delete[] stream;

	if (command != nullptr) {
		commands.push_back(command);
	}

	return commands;
}

void FreeCommands(CommandList commands)
{
	for (auto iter = commands.begin(); iter != commands.end(); ++iter) {
		delete *iter;
	}
}

void RegisterCommand(std::string name, command_cb cb)
{
	gCommandLookup.insert({ name, cb });
}

void ProcessCommands(CommandList commands)
{
	for (auto iter = commands.begin(); iter != commands.end(); ++iter)
	{
		Command *command = *iter;
		if (gCommandLookup.find(command->name) != gCommandLookup.end()) {
			gCommandLookup[command->name](command);
		}
	}
}

Command::Command(char *n)
{
	// move pass the COMMAND_INDICATOR
	++n;
	name = Trim(n);
}
