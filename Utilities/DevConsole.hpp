#pragma once
#include "Engine/Math/raw_vector.hpp"
#include <string>
#include <deque>
#include <map>
#include <sstream>
#include <vector>

#include "Engine/Utilities/EngineCommon.hpp"
#include <fstream>

class XMLFontRenderer;
struct HtmlLogManager;
class XMLFontRenderer;

struct ConsoleCommandArgs {
	ConsoleCommandArgs(const std::string& args);
	std::vector<std::string> m_argsList;
	std::string m_args;
};

typedef void(ConsoleFunction)(const ConsoleCommandArgs&);

class DevConsole
{
public:
	DevConsole(void);
	~DevConsole(void);
	static void ConsolePrint(const std::string& line, const std::vector<std::pair<RGBA, int>>& color);
	void ExecuteConsoleString(std::string& console);
	void RegisterFunction(const std::string& funcName, ConsoleFunction* func);
	void Render();
	void ToggleVisibility();
	static void ConsolePrintf(const char *fmt, RGBA color, ...);
	static void vConsolePrintf(const char* fmt, RGBA color, va_list args);
	void SubmitCommand();
	void ClearCommand();
	void PopCharFromCommand();
	void AppendChar(unsigned char asKey);
	static int GetNumWords(std::string strToTest);
	void DrawBlinkingCursor();
	void DrawConsoleBox();
	static DevConsole* GetInstance();
public:
	int m_frameCounter;
	float m_lineHeight;
	bool m_enabled;
	std::string m_commandPrompt;
	bool m_logToDisk;
	XMLFontRenderer* m_fontRenderer;
};

struct ConsoleLine {
	ConsoleLine(std::string line, std::vector<std::pair<RGBA, int> > m_wordColorPairs) : m_wordColorPairs(m_wordColorPairs), line(line) {}
	std::vector<std::pair<RGBA, int> > m_wordColorPairs;
	std::string line;
	bool operator== (const ConsoleLine& rhs) const {
		if (rhs.line != line) return false;
		if (rhs.m_wordColorPairs != m_wordColorPairs) return false;
		return true;
	}
};

void CommandTestSend(const ConsoleCommandArgs&);
void CommandForceTest(const ConsoleCommandArgs&);
size_t NetAddressForHost(NetAddress* outbuf, size_t outbuf_len, int family, const std::string& hostname, uint16_t port, bool bindable);




