#include "DevConsole.hpp"
#include <iosfwd>
#include <iterator>
#include <ctime>
#include <iostream>
#include <algorithm>

#include <windows.h>
#include <gl/GL.h>

#include "Engine/Utilities/EngineCommon.hpp"
#include "Engine/Utilities/XMLFontRenderer.hpp"
#include "../Graphics/Renderer.hpp"
#include "../Assert.hpp"
#include "../NetAddress.hpp"
#include "../NetSession.hpp"
#include "../NetMessage.hpp"
#include "../NetworkingCommon.hpp"

static std::deque<ConsoleLine> g_consoleLines;
static std::map<std::string, ConsoleFunction*> g_messageDefinitions;
static NetSession* g_gameSession = nullptr;

/*
// Add a new message - wherever you're doing this currently


*/


const float g_lineSpaceMult = 1.f;

struct HtmlLogManager {

	std::ofstream m_outfile;

	HtmlLogManager() : m_outfile("ConsoleLogFile.html") {
		m_outfile << "<html><body>" << std::endl;
	}
	~HtmlLogManager() {
		m_outfile << "</body></html>";
		m_outfile.flush();
		m_outfile.close();
	}

	void WriteConsoleLine(const ConsoleLine& currentConsoleLine) {
		std::string delimiter = " ";
		std::string currentLineText = currentConsoleLine.line;

		std::vector<std::string> splitConsoleText;
		size_t n = (size_t)-1;

		for (;;) {
			if (currentLineText.empty()) break;
			n = currentLineText.find(delimiter);
			std::string token = currentLineText.substr(0, n);
			splitConsoleText.push_back(token);
			currentLineText.erase(0, n + delimiter.length());
			if (n == std::string::npos) break;
		}

		TiXmlDocument doc;
		std::vector<std::pair<RGBA, int>> color = currentConsoleLine.m_wordColorPairs;

		int wordCounter;

		for (std::vector<std::pair<RGBA, int>>::iterator it = color.begin(); it != color.end(); ++it) {
			RGBA wordColor = it->first;
			wordCounter = 0;

			std::pair<RGBA, int> consoleLineColorOffsetPair = *it;;
			for (int numWords = 0; numWords < consoleLineColorOffsetPair.second; numWords++) {
				TiXmlElement* divTag = new TiXmlElement("div");
				TiXmlText* text = new TiXmlText(splitConsoleText[wordCounter].c_str());
				wordCounter++;

				divTag->LinkEndChild(text);

				doc.LinkEndChild(divTag);
				std::stringstream colorString;
				colorString << "color: rgb(";
				colorString << ((int)wordColor.r()) << "," << ((int)wordColor.g()) << "," << ((int)wordColor.b());
				colorString << ");";
				colorString << "display: inline;";
				divTag->SetAttribute("style", colorString.str().c_str());
			}
		}

		TiXmlElement* theBreak = new TiXmlElement("br");
		doc.LinkEndChild(theBreak);
		TiXmlPrinter printer;
		printer.SetIndent("    ");

		doc.Accept(&printer);
		std::string xmltext = printer.CStr();

		m_outfile << xmltext << std::endl;
	}
};

static HtmlLogManager g_logger;

void CommandClear(const ConsoleCommandArgs&) {
	for (std::deque<ConsoleLine>::iterator it = g_consoleLines.begin(); it != g_consoleLines.end();) {
		ConsoleLine current = *it;
		size_t foundLoc = current.line.find('>');
		if (foundLoc != 0 || foundLoc == std::string::npos) {
			it = it = g_consoleLines.erase(it);
		}
		else {
			++it;
		}
	}
}

void CommandGenerateFiles(const ConsoleCommandArgs&) {

}

void CommandHelp(const ConsoleCommandArgs&) {
	for(std::map<std::string, ConsoleFunction*>::iterator it = g_messageDefinitions.begin(); it != g_messageDefinitions.end(); ++it) {
		std::vector<std::pair<RGBA, int>> helpColor;
		helpColor.push_back(std::make_pair(RGBA(0,255,0,255), 1));
		std::string commandName = it->first;
		DevConsole::ConsolePrint(commandName, helpColor);
	}
}

void CommandQuit(const ConsoleCommandArgs&) {
	exit(1);
}

void CommandTestSend(const ConsoleCommandArgs& args) {
	(void)args;
// 	int numMessages = atoi(args.m_argsList[1].c_str());
// 
// 	for (int index = 0; index < numMessages; index++) {
// 		NetMessage* msg = new NetMessage(NetMessage::GetNetMessageDefinitionByName("ping")->m_id);
// 
// 		std::string stringData = "TESTSEND MESSAGE " ;
// 		stringData += (char)index;
// 
// 		size_t dataLength = stringData.size();
// 
// 		msg->SetMessageData((void*)stringData.c_str(), dataLength);
// 		g_gameSession->SendMsg(msg, TODO);
// 	}
}

void CommandHostSession(const ConsoleCommandArgs& ca) {
	g_gameSession->m_me->m_connectionIndex = 0;
	g_gameSession->m_hostConnection = g_gameSession->m_me;
	g_gameSession->Listen(true);

	DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Hosting session\n");
	(void)ca;
}


void CommandForceTest(const ConsoleCommandArgs&) {
	g_gameSession->ForceTestAll();
}


void CommandJoinSession(const ConsoleCommandArgs& ca) {

	std::string ip = ca.m_argsList[1];
	short port = (short)atoi(ca.m_argsList[2].c_str());

	NetAddress netAddr;
	memcpy(&netAddr.m_addr, ip.c_str(), ip.size());
	netAddr.m_port = port;
	NetConnection* newConn = g_gameSession->AddConnection(netAddr, true);

	NetMessage* msg = new NetMessage(NetMessage::GetNetMessageDefinitionByName("joinrequest")->m_id);
	msg->SetMessageData("CHRISTIAN WALKER", sizeof("CHRISTIAN WALKER"));
	g_gameSession->SendMsg(msg, newConn);
}

void CommandCreateSession(const ConsoleCommandArgs& args) {
	short port = (short)atoi(args.m_argsList[1].c_str());
	g_gameSession = NetSystem::GetInstance()->CreateSession(port);

	DevConsole::ConsolePrintf("%s %i", RGBA(0, 255, 0, 255), "Session created a on port", port);

	g_gameSession->Listen(true);
	g_gameSession->Host(port);

}

void CommandPing(const ConsoleCommandArgs& args) {
		FATAL_ASSERT(args.m_argsList.size() == 4);

		unsigned short port = (unsigned short)atoi(args.m_argsList[2].c_str());
		std::string ip = args.m_argsList[1];

		//construct packet, put the net message in it, push back to outgoing queue
		NetAddress toAddr;
		memcpy(toAddr.m_addr, ip.data(), ip.size());
		toAddr.m_port = port;

		NetPacket* packet = new NetPacket();

		packet->m_address = toAddr;
		NetMessage msg(NetMessage::GetNetMessageDefinitionByName("ping")->m_id);

		const char* stringData = args.m_argsList[3].c_str();
		size_t dataLength = strlen(stringData);
		msg.SetMessageData((void*)stringData, dataLength);
		packet->AddMessage(msg);
		
		packet->UpdateNumMessages();

		g_gameSession->SendPacket(packet);
}



DevConsole::DevConsole(void)
{
	m_logToDisk = true;
	m_enabled = false;
	m_fontRenderer = new XMLFontRenderer();
	m_fontRenderer->LoadFont("Fonts/Calibri.fnt");
	m_lineHeight = 20.f;
	m_frameCounter = 0;

// 	RegisterFunction(std::string("clear"), CommandClear);
// 	RegisterFunction(std::string("help"), CommandHelp);
// 	RegisterFunction(std::string("quit"), CommandQuit);
// 	RegisterFunction(std::string("ping"), CommandPing);
// 	RegisterFunction(std::string("createsession"), CommandCreateSession);
// 	RegisterFunction(std::string("host_session"), CommandHostSession);
// 	RegisterFunction(std::string("join_session"), CommandJoinSession);
// 	RegisterFunction(std::string("force_test"), CommandForceTest);
// 	RegisterFunction(std::string("test_send"), CommandTestSend);


	//ExecuteConsoleString(std::string("help"));
	//ConsolePrintf("%s %d + %d = %d", RGBA(100,0,255,255),"Consoleprintf", 1, 2, 3);
	//std::vector<std::pair<RGBA, int>> hiColors;
	//hiColors.push_back(std::make_pair(RGBA(255,255,255,255), 1));
	//ConsolePrint("HIIIIIIII!", hiColors); 
}


ConsoleCommandArgs::ConsoleCommandArgs(const std::string& args) {

	std::string s = std::string(args);
	std::string delimiter = " ";
	size_t delimiterFoundIndex = (size_t)-1;
	m_args = args;

	for(;;) {
		if (s.empty()) break;
		delimiterFoundIndex = s.find(delimiter);
		std::string token = s.substr(0, delimiterFoundIndex);
		m_argsList.push_back(token);
		s.erase(0, delimiterFoundIndex + delimiter.length());
		if (delimiterFoundIndex == std::string::npos) break;
	}

}

void DevConsole::DrawConsoleBox() {

	std::vector<Vertex> consoleVertices;
	Renderer& myRenderer = Renderer::GetInstance();

	Vertex consoleVertex;
	consoleVertex.m_color = RGBA(25, 25, 25, 155);

	consoleVertex.m_position = Vec3(0.f, 0.f, 0.f);
	consoleVertices.push_back(consoleVertex);

	consoleVertex.m_position = Vec3((float)SCREEN_WIDTH, 0.f, 0.f);
	consoleVertices.push_back(consoleVertex);

	consoleVertex.m_position = Vec3((float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.f);
	consoleVertices.push_back(consoleVertex);

	consoleVertex.m_position = Vec3(0.f, (float)SCREEN_HEIGHT, 0.f);
	consoleVertices.push_back(consoleVertex);

	myRenderer.RenderPrimitives(GL_QUADS, consoleVertices);
}

DevConsole* DevConsole::GetInstance()
{
	DevConsole* devConsole = new DevConsole();
	return devConsole;
}

void DevConsole::ConsolePrintf(const char *fmt, RGBA color, ...) {
	va_list args;
	va_start(args, color);
	vConsolePrintf(fmt, color, args);
	va_end(args);
}

int DevConsole::GetNumWords(std::string strToTest)
{
	std::string delimiter = " ";
	size_t delimiterFoundIndex = (size_t)-1;

	int numFound = 0;
	for(;;) {
		if (strToTest.empty()) break;
		delimiterFoundIndex = strToTest.find(delimiter);
		strToTest.erase(0, delimiterFoundIndex + delimiter.length());
		numFound++;
		if (delimiterFoundIndex == std::string::npos) break;
	}
	return numFound;
}

void DevConsole::vConsolePrintf(const char* fmt, RGBA color, va_list args) {
	char theStr[1024] = {'\0'};
	vsnprintf(theStr, 127, fmt, args);
	std::vector<std::pair<RGBA, int>> vPrintfColor;
	std::string testNumWords = std::string(theStr);
	int numWords = GetNumWords(testNumWords);
	vPrintfColor.push_back(std::make_pair(color,  numWords));
	ConsolePrint(theStr, vPrintfColor);
}

void DevConsole::RegisterFunction(const std::string& funcName, ConsoleFunction* func) {
	g_messageDefinitions.insert(std::make_pair(funcName, func));
}

void DevConsole::ExecuteConsoleString(std::string& console) {
	NamedProperties np;

	ConsoleCommandArgs cargs(console);
	 
	for (unsigned int index = 1; index < cargs.m_argsList.size(); index++) {
		np.Set(std::to_string(index), cargs.m_argsList[index]);
	}

	FireEvent(cargs.m_argsList[0], np, true);

// 	if (g_messageDefinitions.find(cargs.m_argsList[0]) == g_messageDefinitions.end())
// 	{
// 		return;
// 	}
// 
// 	std::string command = cargs.m_argsList[0]; 
// 	g_messageDefinitions[command](cargs);

	//std::vector<std::pair<RGBA, int>> commandLogColorPair;
	//std::string consoleLineChar = "> ";
	//console = consoleLineChar.append(console);
	//commandLogColorPair.push_back(std::make_pair(RGBA(255, 255, 255, 255), GetNumWords(console)));
	//ConsolePrint(console, commandLogColorPair);
}

void DevConsole::ConsolePrint(const std::string& line, const std::vector<std::pair<RGBA, int>>& color) {
	ConsoleLine cline(line, color);
	g_consoleLines.push_front(cline);
	g_logger.WriteConsoleLine(cline);
}

void DevConsole::Render() {

	if (m_enabled) {

		Renderer& myRenderer = Renderer::GetInstance();
		myRenderer.SetIdentity();
		myRenderer.SetOrtho(0.0, SCREEN_WIDTH, 0.0, SCREEN_HEIGHT, 0, 1);
		glDisable(GL_DEPTH_TEST);

		m_frameCounter++;
		if (m_enabled) {
			DrawConsoleBox();

			Vec3 position = Vec3(0.f, 5.f, 0.f);

			int currentLine = 0;

			if (!m_commandPrompt.empty()) {
				std::vector<std::pair<RGBA, int> > promptColorUniform;
				promptColorUniform.push_back(std::make_pair(RGBA(255, 255, 255, 255), GetNumWords(m_commandPrompt)));

				ConsoleLine cline(m_commandPrompt, promptColorUniform);
				g_consoleLines.push_front(cline);
			}

			for (std::deque<ConsoleLine>::iterator it = g_consoleLines.begin(); it != g_consoleLines.end(); ++it) {
				ConsoleLine& currentConsoleLine = *it;

				int currentWord = 0;
				std::string delimiter = " ";
				std::string consoleLineText = currentConsoleLine.line;

				std::vector<std::string> splitConsoleText;
				size_t delimiterFoundIndex = (size_t)-1;

				for (;;) {
					if (consoleLineText.empty()) break;
					delimiterFoundIndex = consoleLineText.find(delimiter);
					std::string token = consoleLineText.substr(0, delimiterFoundIndex);
					splitConsoleText.push_back(token);
					consoleLineText.erase(0, delimiterFoundIndex + delimiter.length());
					if (delimiterFoundIndex == std::string::npos) break;
				}

				float widthOfSpace = m_fontRenderer->CalcTextWidth(" ", m_lineHeight);
				for (std::vector<std::pair<RGBA, int>>::iterator it2 = currentConsoleLine.m_wordColorPairs.begin(); it2 != currentConsoleLine.m_wordColorPairs.end(); ++it2) {
					std::pair<RGBA, int> lineColorToCutoffPair = *it2;
					RGBA startOfNextColor = lineColorToCutoffPair.first;

					for (int c = 0; c < lineColorToCutoffPair.second; c++) {
						m_fontRenderer->DrawString(splitConsoleText[currentWord], startOfNextColor, position, m_lineHeight);
						float width = m_fontRenderer->CalcTextWidth(splitConsoleText[currentWord], m_lineHeight);
						position.x() += width + widthOfSpace;
						currentWord++;
					}
				}

				position.x() = 0;
				position = position + Vec3(0.f, m_lineHeight * g_lineSpaceMult, 0.f);
				++currentLine;
			}


			m_fontRenderer->m_lastCursor = Vec2(0.f, 0.f);
			m_fontRenderer->m_cursor = Vec2(0.f, 0.f);

			//DrawConsoleBox();

			if (!m_commandPrompt.empty()) {
				g_consoleLines.pop_front();
			}
			else {
				return;
			}

			if (m_frameCounter < 50)
				DrawBlinkingCursor();
			else if (m_frameCounter > 100)
				m_frameCounter = 0;

		}

		glEnable(GL_DEPTH_TEST);
	}

}

void DevConsole::ToggleVisibility() {
	m_enabled = !m_enabled;
}

void DevConsole::DrawBlinkingCursor() {
	float startXoffset = m_fontRenderer->CalcTextWidth(m_commandPrompt, m_lineHeight);
	//float startXoffset = 0.f;
	Vec2 startPosition = m_fontRenderer->m_lastCursor;
	startPosition.x() += startXoffset;
 
	float endXoffset = m_fontRenderer->CalcTextWidth(" ", m_lineHeight);
	Vec2 endPosition = Vec2(startPosition.x() + endXoffset, startPosition.y() + m_lineHeight);

	std::vector<Vertex> blinkingCursorVertices;

	Vertex cursorPos;
	cursorPos.m_color = RGBA(0, 255, 0, 255);

	cursorPos.m_position = Vec3(startPosition.x(), startPosition.y(), 0.f);
	blinkingCursorVertices.push_back(cursorPos);

	cursorPos.m_position = Vec3(startPosition.x(), endPosition.y(), 0.f);
	blinkingCursorVertices.push_back(cursorPos);

	cursorPos.m_position = Vec3(endPosition.x(), endPosition.y(), 0.f);
	blinkingCursorVertices.push_back(cursorPos);

	cursorPos.m_position = Vec3(endPosition.x(), startPosition.y(), 0.f);
	blinkingCursorVertices.push_back(cursorPos);

	Renderer& myRenderer = Renderer::GetInstance();
	myRenderer.RenderPrimitives(GL_QUADS, blinkingCursorVertices);

}


DevConsole::~DevConsole(void)
{
}

void DevConsole::SubmitCommand()
{
	if (m_commandPrompt.empty()) return;
	std::string commandToProcess = m_commandPrompt;
	std::transform(commandToProcess.begin(), commandToProcess.end(), commandToProcess.begin(), ::tolower);
	ExecuteConsoleString(commandToProcess);
}

void DevConsole::ClearCommand()
{
	if (m_commandPrompt.empty()) m_enabled = false;
	m_commandPrompt.clear();
}

void DevConsole::PopCharFromCommand()
{
	if (!m_commandPrompt.empty()) {
		m_commandPrompt.pop_back();
	}
}

void DevConsole::AppendChar(unsigned char asKey)
{
	if (asKey != '`') 
		m_commandPrompt.push_back(asKey);
}
