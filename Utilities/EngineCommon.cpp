#define ENABLEPRINTS

#ifdef WIN32 
#include <Windows.h>
#include <io.h>
#pragma comment( lib, "opengl32" ) // Link in the OpenGL32.lib static library
#pragma comment( lib, "glu32" )
#pragma comment(lib, "Ws2_32")
#endif

#include <stdio.h>

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <fstream>
#include <iostream>

#include "Engine/Utilities/EngineCommon.hpp"
#include "../Math/Vertex.hpp"
#include <limits>
#include "../NetAddress.hpp"
#include <ws2tcpip.h>
#include "../Assert.hpp"
#include "../EventSystem.hpp"


bool g_isQuitting = false;

static const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;

// COMMENT: The concept of this wrapper is a thin, platform agnostic API that hooks into API-specific functions so that you don't need to have "#ifdef _WIN32" all over the place
Win32Wrapper myWinWrapper;

/*
void ReportAllocations()
{
#ifdef WIN32
	for (auto it = g_allocationTracker.begin(); it != g_allocationTracker.end(); ++it) {
		ByteTrackerData btd = it->second;
		char buffer[1024];
		sprintf(buffer, "%s %s %d %s %s %s %d %s %d %s %s %s", btd.m_file, "(", btd.m_lineNum, ")", ":", "Memory leak of", btd.m_bytesAllocated, "bytes, allocated on line ", btd.m_lineNum, "of", btd.m_file, "\n");
		OutputDebugStringA(buffer);
	}

	std::stringstream ss;
	ss << "Average Byes Allocated: " << g_overallAllocationData.m_averageAllocation << std::endl;
	ss << "Total Bytes Allocated: " << g_overallAllocationData.m_totalBytesAllocated << std::endl;
	ss << "Largest Allocation: " << g_overallAllocationData.m_largestAllocation << std::endl;
	ss << "Number of Allocations" << g_overallAllocationData.m_totalAllocations << std::endl;

	std::string asString = ss.str();
	OutputDebugStringA(asString.c_str());

#endif
}
*/



// get sockaddr, IPv4 or IPv6:
 void* GetInAddr(sockaddr const *sa)
{
	return &(((sockaddr_in*)sa)->sin_addr);
}

 void* GetInAddr(sockaddr *sa, uint16_t *port, size_t *size_out)
 {
	 *size_out = sizeof(((sockaddr_in*)sa)->sin_addr);
	 *port = ((sockaddr_in*)sa)->sin_port;
	 return &(((sockaddr_in*)sa)->sin_addr);
 }

void NetAddrFromSockAddr(NetAddress *na, sockaddr *addr)
{
	memset(na, 0, sizeof(NetAddress));
	size_t addrlen;
	void *addr_ptr = GetInAddr(addr, &(na->m_port), &addrlen);
	FATAL_ASSERT(addrlen <= sizeof(na->m_addr));
	memcpy(&na->m_addr, addr_ptr, addrlen);
}


//------------------------------------------------------------------------
void SockAddrFromNetAddr(sockaddr *addr, size_t *addrlen, NetAddress const &net_addr)
{
	sockaddr_in* sa = (sockaddr_in*)addr;
	memset(sa, 0, sizeof(sockaddr_in));
	sa->sin_family = AF_INET;
	inet_pton(AF_INET, (PCSTR)net_addr.m_addr, &sa->sin_addr);
	*addrlen = sizeof(sockaddr_in);
	sa->sin_port = htons(net_addr.m_port);
}

addrinfo* AllocAddressesForHost(char const *host,
	char const *service,
	int family,
	int socktype,
	bool binding)
{
	addrinfo hints;
	addrinfo *addr;

	if (nullptr == host) {
		host = "localhost";
	}

	// Which network layer it's using - usually want to UNSPEC, since it doesn't matter.  But since we're hard coding
	// the client sides connection, we will likely want to use AF_INET when we want to bind an address

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = family;

	hints.ai_socktype = socktype; // STREAM based, determines transport layer (TCP)
	hints.ai_flags = binding ? AI_PASSIVE : 0; // used for binding/listening

	int status = getaddrinfo(host, service, &hints, &addr);
	if (status != 0) {
		return nullptr;
	}

	return addr;
}

//-------------------------------------------------------------------------------------------------------
void FreeAddresses(addrinfo *addresses)
{
	freeaddrinfo(addresses);
}

//-------------------------------------------------------------------------------------------------------
uint16_t GetAddressPort(sockaddr const *sa)
{
	USHORT port = 0;
	port = (((sockaddr_in*)sa)->sin_port);
	return ntohs(port);
}

//-------------------------------------------------------------------------------------------------------
size_t GetAddressName(char *buffer, size_t const buffer_size, sockaddr const *sa)
{
	char addr_name[INET6_ADDRSTRLEN];
	memset(addr_name, 0, sizeof(addr_name));
	inet_ntop(sa->sa_family, GetInAddr(sa), addr_name, INET6_ADDRSTRLEN);

	uint16_t port = GetAddressPort(sa);

	//size_t len = min(buffer_size - 1, strlen(addr_name));
	return sprintf_s(buffer, buffer_size, "%s:%i", addr_name, port);
}

//-------------------------------------------------------------------------------------------------------
void ForEachAddress(addrinfo *addresses, address_work_cb cb, void *user_arg)
{
	addrinfo *iter = addresses;
	while (nullptr != iter) {
		if (cb(iter, user_arg)) {
			break;
		}

		iter = iter->ai_next;
	}
}


bool IsLittleEndian() {
	short number = 0x1;
	char *numPtr = (char*)&number;
	return (numPtr[0] == 1);
}


float RandomAngle() {
	return RandomFloat(0.f,360.f);
}

Vec3 Vec2iToVec3(const Vec2i& inVec2i) {

	float xAsFloat = (float)inVec2i.x();
	float yAsFloat = (float)inVec2i.y();
	float zAsFloat = 0.f;
	
	return Vec3(xAsFloat, yAsFloat, zAsFloat);
}

std::vector<Vertex> SquareFromPoint(const Vec3& bottomLeftPoint, float size, const RGBA& color, const Vec3& insetVector) {
	std::vector<Vertex> square(4);
	
	Vec3 bottomLeftInsetVector = insetVector;
	Vec3 bottomRightInsetVector = Vec3(-insetVector.x(), insetVector.y(), 0.f);
	Vec3 topLeftInsetVector = Vec3(insetVector.x(), -insetVector.y(), 0.f);
	Vec3 topRightInsetVector = Vec3(-insetVector.x(), -insetVector.y(), 0.f);

	Vertex aVertex;
	aVertex.m_position = bottomLeftPoint + bottomLeftInsetVector; //bottom left
	aVertex.m_color = color;
	square[3] = aVertex;

	aVertex.m_position = bottomLeftPoint + Vec3(0.f, size, 0.f) + topLeftInsetVector; //top left
	aVertex.m_color = color;
	square[2] = aVertex;

	aVertex.m_position = bottomLeftPoint + Vec3(size, size, 0.f) + topRightInsetVector; //top right
	aVertex.m_color = color;
	square[1] = aVertex;

	aVertex.m_position = bottomLeftPoint + Vec3(size, 0.f, 0.f) + bottomRightInsetVector; //bottom right
	aVertex.m_color = color;
	square[0] = aVertex;

	return square;
}

float RandomFloat(float min, float max)
{
	return std::uniform_real_distribution<float>(min, max)(g_rng);
}

unsigned char RandomUnsignedChar(unsigned char min, unsigned char max) {
	return (unsigned char)std::uniform_int_distribution<unsigned int>(min, max)(g_rng);
}

int RandomInt(int min, int max)
{
	return std::uniform_int_distribution<int>(min, max)(g_rng);
}

int RandomInt(int min, int max, RngT& rng)
{
	return std::uniform_int_distribution<int>(min, max)(rng);
}

bool IsWhitespace(char c)
{
		switch (c) {
		case '\n':
		case '\r':
		case ' ':
		case '\t':
			return true;

		default:
			return false;
		};
}

char* Trim(char *s)
{
	while (IsWhitespace(*s)) {
		++s;
	}

	char *end = s;
	while (*end != NULL) {
		++end;
	}

	if (end > s) {
		--end;
		while ((end > s) && IsWhitespace(*end)) {
			--end;
		}

		++end;
		*end = NULL;
	}

	return s;
}

//-----------------------------------------------------------------------------------------------
// Construct a temporary std::string using printf-style formatting.  For example:
//	std::string studentInfoString = Stringf( "%s is a %i-year-old %s.", studentName, studentAge, studentGender );
//By Squirrel Eiserloh
//
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}

template<typename T>
bool ValueInRange(const T& start, const T& end, const T& value) {
	return !(value < start || end < value);
}


std::vector<std::string> SplitString(const std::string& input, const std::string& delimiter)
{
	std::vector<std::string> output;
	size_t start = 0;
	size_t end = 0;

	while (start != std::string::npos && end != std::string::npos)
	{
		start = input.find_first_not_of(delimiter, end);

		if (start != std::string::npos)
		{
			end = input.find_first_of(delimiter, start);

			if (end != std::string::npos)
			{
				output.push_back(input.substr(start, end - start));
			}
			else
			{
				output.push_back(input.substr(start));
			}
		}
	}

	return output;

}

void FindAllFilesOfType(const std::string& directory, const std::string& searchPattern, std::vector<std::string>& out_filesFound) {
	int error = 0;
	struct _finddata_t fileInfo;


	std::string file = directory + "/" + searchPattern;
	intptr_t searchHandle = _findfirst(file.c_str(), &fileInfo);

	while (searchHandle != -1 && !error) {
		bool isADirectory = fileInfo.attrib & _A_SUBDIR ? true : false;

		if (!strcmp(fileInfo.name, ".") || !strcmp(fileInfo.name, "..")) {
			//ignore special cases
		}
		else if (isADirectory) {
		}
		else
			out_filesFound.push_back(directory + fileInfo.name);

		error = _findnext(searchHandle, &fileInfo);
	}

	_findclose(searchHandle);
}

unsigned char StringToUnsignedChar(const std::string& c) {
	return (unsigned char)c[0];
}

RGBA ExtractRGBA(const std::string& attribute) {
	std::vector<std::string> attribSplit = SplitString(attribute, ",");
	assert(attribSplit.size() == 3 || attribSplit.size() == 4);

	RGBA resultColor;
	resultColor = RGBA((unsigned char)atoi(attribSplit[0].c_str()), (unsigned char)atoi(attribSplit[1].c_str()), (unsigned char)atoi(attribSplit[2].c_str()), 255);
	return resultColor;
}

Vec2i ExtractVec2i(const std::string& attribute){
	std::vector<std::string> attribSplit = SplitString(attribute, ",");
	assert(attribSplit.size() == 2);
	Vec2i resultVec2i;
	resultVec2i = Vec2i(atoi(attribSplit[0].c_str()), atoi(attribSplit[1].c_str()));
	return resultVec2i;
}


std::string RGBAtoString(const RGBA& rgba) {

	std::stringstream rSS;

	rSS << (unsigned int)rgba.r();
	rSS << ",";

	rSS << (unsigned int)rgba.g();
	rSS << ",";

	rSS << (unsigned int)rgba.b();
	rSS << ",";

	rSS << (unsigned int)rgba.a();

	std::string myStr = rSS.str();
	return myStr;
}

std::string Vec3ToString(const Vec3& pos) {
	std::stringstream stringConv;
	stringConv << pos.x();
	stringConv << ",";
	
	stringConv << pos.y();
	stringConv << ",";

	stringConv << pos.z();

	std::string retString = stringConv.str();
	return retString;
}

std::string Vec2iToString(const Vec2i& pos) {
	std::stringstream stringConv;
	stringConv << pos.x();
	stringConv << ",";

	stringConv << pos.y();

	std::string retString = stringConv.str();
	return retString;
}

int Distance(int x1, int x2, int y1, int y2)
{
	int beforeroot = ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
	if (beforeroot > 0)
	{
		return (int)sqrt(beforeroot);
	}
	else if (beforeroot < 0)
	{
		return (int)sqrt(-beforeroot);
	}
	else
	{
		return 0;
	}
}


RGBA RandomColor() {
	unsigned char r = RandomUnsignedChar(0, 255);
	unsigned char g = RandomUnsignedChar(0, 255);
	unsigned char b = RandomUnsignedChar(0, 255);
	unsigned char a = 255;
	return RGBA(r, g, b, a);
}

//-----------------------------------------------------------------------------------------------
// Same as the above version of Stringf(), but with an explicitly chosen maximum buffer size.
//
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

size_t GetFileLength(FILE* openFile) {
	fseek(openFile, 0L, SEEK_END);
	size_t fileLength = ftell(openFile);
	fseek(openFile, 0L, SEEK_SET);
	return fileLength;
}

size_t GetFileLength(const std::string& filePath) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0)
		assert(false);

	fseek(fp, 0L, SEEK_END);
	size_t fileLength = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return fileLength;
}

bool LoadFileToNewBinaryBuffer(const std::string& filePath, unsigned char* &out_binaryBuffer, size_t& out_bufferSize) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0) {
		return false;
	}

	out_bufferSize = GetFileLength(fp);
	out_binaryBuffer = new unsigned char[out_bufferSize];

	size_t numBytesRead = fread(out_binaryBuffer, 1, out_bufferSize, fp);
	if (numBytesRead != out_bufferSize) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool LoadFileToVector(const std::string& filePath, std::vector< unsigned char >&out_dataVector) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0) {
		return false;
	}

	size_t fileSizeBytes = GetFileLength(fp);

	size_t currentVectorSize = out_dataVector.size();
	out_dataVector.resize(fileSizeBytes);

	size_t numBytesRead = fread(&out_dataVector.data()[currentVectorSize], 1, fileSizeBytes, fp);

	if (numBytesRead <= 0) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;

}

std::stringstream GetFileContents(const std::string& filename)
{
	std::ifstream in(filename);
	if (in)
	{
		std::stringstream contents;
		contents << in.rdbuf();
		in.close();
		return contents;
	}
	throw(errno);
}


bool FlushTextBufferToDisk(const std::string& filePath, const char* textDataBuffer) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "wb");

	if (errorCode != 0) {
		return false;
	}

	size_t stringLength = strlen(textDataBuffer);
	size_t numBytesWritten = fwrite(textDataBuffer, 1, stringLength, fp);

	if (numBytesWritten != stringLength) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool FlushByteBufferToDisk(const std::string& filePath, const unsigned char* byteBuffer, size_t bufferSize) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "wb");

	if (errorCode != 0) {
		return false;
	}

	size_t numBytesWritten = fwrite(byteBuffer, 1, bufferSize, fp);

	if (numBytesWritten != bufferSize)
		return false;

	fclose(fp);
	return true;
}

bool FlushByteVectorToDisk(const std::string& filePath, const std::vector< unsigned char >& byteVector) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "wb+");

	if (errorCode != 0) {
		return false;
	}

	size_t byteVectorSize = byteVector.size();
	size_t numBytesWritten = fwrite(&byteVector.data()[0], 1, byteVectorSize, fp);

	if (numBytesWritten != byteVectorSize) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

bool LoadFileToExistingBinaryBuffer(const std::string& filePath, char* &out_textBuffer, size_t bufferSizeLimit) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0) {
		return false;
	}

	size_t fileSizeBytes = GetFileLength(fp);
	size_t numBytesRead = fread_s(out_textBuffer, bufferSizeLimit, 1, fileSizeBytes, fp);

	if (numBytesRead <= 0) {
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}



bool LoadFileToNewTextBuffer(const std::string& filePath, char* &out_textBuffer)
{
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0)
		return false;

	size_t numBytes = GetFileLength(fp);
	out_textBuffer = new char[numBytes + 1];
	size_t numBytesRead = fread(out_textBuffer, 1, numBytes, fp);

	if (numBytesRead <= 0) {
		fclose(fp);
		return false;
	}

	out_textBuffer[numBytes] = '\0';
	fclose(fp);
	return true;
}


bool FlushStringToDisk(const std::string& filePath, const std::string& stringToWrite) {
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "wb");

	if (errorCode != 0) {
		return false;
	}

	size_t numBytesWritten = fwrite(&stringToWrite[0], 1, stringToWrite.size(), fp);
	if (numBytesWritten != stringToWrite.size()) {
		fclose(fp);
		return false;
	}
		
	fclose(fp);
	return true;
}


void FireEvent(const std::string& eventName, NamedProperties& args /*= NamedProperties()*/)
{
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.FireEvent(eventName, args);
}

void RegisterEventCallback(const std::string& eventName, EventCallback* callbackFunc)
{
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.RegisterEvent(eventName, callbackFunc);
}

void UnregisterEventCallback(const std::string& eventName, EventCallback* callbackFunc)
{
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.UnregisterEvent(eventName, callbackFunc);
}

bool LoadFileToExistingTextBuffer(std::string& filePath, char* &out_textBuffer, size_t bufferSizeLimit)
{
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0) {
		return false;
	}

	size_t fileSizeBytes = GetFileLength(fp);
	size_t numBytesRead = fread_s(out_textBuffer, bufferSizeLimit, 1, fileSizeBytes, fp);

	if (numBytesRead <= 0) {
		fclose(fp);
		return false;
	}

	fclose(fp);

	out_textBuffer[fileSizeBytes] = '\0';
	return true;
}


bool LoadFileToExistingString(const std::string& filePath, std::string& out_string)
{
	FILE* fp;
	errno_t errorCode = fopen_s(&fp, filePath.c_str(), "rb");

	if (errorCode != 0) {
		return false;
	}

	size_t fileSizeBytes = GetFileLength(fp);
	out_string.resize(fileSizeBytes);

	size_t numBytesRead = fread(&out_string[0], 1, fileSizeBytes, fp);
	out_string[fileSizeBytes] = '\0';

	if (numBytesRead <= 0) {
		fclose(fp);
		return false;
	}

	fclose(fp);

	return true;
}



void GetFilesList(const std::string& dir, std::vector<std::string>& foundFilesOut) {
	int error = 0;
	struct _finddata_t fileInfo;
	intptr_t searchHandle = _findfirst(dir.c_str(), &fileInfo);
	while (searchHandle != 1 && !error) {
		//bool isADirectory = fileInfo.attrib && _A_SUBDIR ? true : false;
		//bool isHidden = fileInfo.attrib & _A_HIDDEN ? true : false;
		foundFilesOut.push_back(fileInfo.name);
		error = _findnext(searchHandle, &fileInfo);
	}
}

void GetFilesRecursively(const std::string& dir, std::vector<std::string>& foundFilesOut) {
	int error = 0;
	struct _finddata_t fileInfo;
	std::string searchPattern = dir + "/*";

	intptr_t searchHandle = _findfirst(searchPattern.c_str(), &fileInfo);
	while (searchHandle != 1 && !error) {
		bool isADirectory = fileInfo.attrib & _A_SUBDIR ? true : false;

		if (!strcmp(fileInfo.name, ".") || !strcmp(fileInfo.name, "..")) {
			//ignore special cases
		}
		else if (isADirectory) {
			std::string recursePath = dir + "/" + fileInfo.name;
			GetFilesRecursively(recursePath, foundFilesOut);
		}
		else 
			foundFilesOut.push_back(dir + "/" + fileInfo.name);

		error = _findnext(searchHandle, &fileInfo);
	}
	_findclose(searchHandle);
}

void ReverseBytes(unsigned char* data, size_t size) {
		unsigned char *istart = data, *iend = istart + size;
		std::reverse(istart, iend);
}


void GenerateFiles(int numFiles, size_t fileSize, const std::string& basePath) {

	CreateDirectoryA(basePath.c_str(), NULL);

	for (int fileNum = 0; fileNum < numFiles; fileNum++) {
		std::vector<unsigned char> data(fileSize * 1000);
		std::stringstream filePathStream;
		filePathStream << basePath << "/" << "RandomFile" << fileNum << ".dat";
		std::string filePath = filePathStream.str();

		for (size_t i = 0; i < data.size(); i++)
			data[i] = RandomInt(0, INT_MAX - 1) % 256;

		FlushByteVectorToDisk(filePath, data);
	}
}

size_t HashBytes(const std::vector<unsigned char>& data) {
	size_t hash = 0;

	for (size_t x = 0; x < data.size(); x++) {
		unsigned char c = data[x];
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
		
	return hash;
}

//-----------------------------------------------------------------------------------------------
// Print a string to the Visual Studio "Output" debug window, if we are running in the debugger.
// Uses printf-style formatting, for example:
//	DebuggerPrintf( "%s is a %i-year-old %s.", studentName, studentAge, studentGender );
// 
void DebuggerPrintf( const char* messageFormat, ... )
{
#if defined(ENABLEPRINTS)
	const int MESSAGE_MAX_LENGTH = 2048;
	char messageLiteral[ MESSAGE_MAX_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, messageFormat );
	vsnprintf_s( messageLiteral, MESSAGE_MAX_LENGTH, _TRUNCATE, messageFormat, variableArgumentList );
	va_end( variableArgumentList );
	messageLiteral[ MESSAGE_MAX_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

#if defined( WIN32 )
	OutputDebugStringA(messageLiteral); // Print the string to the "Output" debug window in Visual Studio
#else
	std::cout << messageLiteral;
#endif

#else
	(void)messageFormat;
#endif;

	//	std::cout << messageLiteral; // Also write it out to standard out, if anyone is listening
}


bool StringEndsWith(const std::string& value, const std::string& ending) {
	if (ending.size() > value.size())
		return false;
    
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool StringBeginsWith(const std::string& value, const std::string& beginning) {
	if (value.find(beginning) == 0)
	{
		return true;
	}
	return false;
}

bool StringReplace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

void StringReplaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
}

inline bool FileExists (const std::string& name) {
	std::ifstream f(name.c_str());
	if (f.good()) {
		f.close();
		return true;
	} else {
		f.close();
		return false;
	}   
}


bool DoesRectOverlapRect(const Vec2& rectOnePos, const Vec2& rectOneSize, const Vec2& rectTwoPos, const Vec2& rectTwoSize) {
	Vec2 distanceToEdge1 = rectOneSize * .5f;
	Vec2 distanceToEdge2 = rectTwoSize * .5f;

	//Edges of rectangle one
	float rightEdge1 = rectOnePos.x() + distanceToEdge1.x();
	float leftEdge1 = rectOnePos.x() - distanceToEdge1.x();
	float topEdge1 = rectOnePos.y() + distanceToEdge1.y();
	float bottomEdge1 = rectOnePos.y() - distanceToEdge1.y();

	//Edges of rectangle two
	float rightEdge2 = rectTwoPos.x() + distanceToEdge2.x();
	float leftEdge2 = rectTwoPos.x() - distanceToEdge2.x();
	float topEdge2 = rectTwoPos.y() + distanceToEdge2.y();
	float bottomEdge2 = rectTwoPos.y() - distanceToEdge2.y();

	bool xIsOverlapping = ValueInRange(leftEdge2, rightEdge2, leftEdge1)||
		ValueInRange(leftEdge1, rightEdge1,leftEdge2);

	bool yIsOverlapping = ValueInRange(bottomEdge1,topEdge1,bottomEdge2)||
		ValueInRange(bottomEdge2,topEdge2,bottomEdge1);

	return xIsOverlapping && yIsOverlapping;
}

bool DoesRectOverlapRect(const RectA& rect1, const RectA& rect2) {
	Vec2 rectOneSize = Vec2(rect1.width,rect1.height);
	Vec2 rectTwoSize = Vec2(rect2.width,rect2.height);
	Vec2 distanceToEdge1 = rectOneSize * .5f;
	Vec2 distanceToEdge2 = rectTwoSize * .5f;

	//Edges of rectangle one
	float rightEdge1 = rect1.xMin + rectOneSize.x();
	float leftEdge1 = rect1.xMin;
	float topEdge1 = rect1.yMin + rectOneSize.y();
	float bottomEdge1 = rect1.yMin;

	//Edges of rectangle two
	float rightEdge2 = rect2.xMin + rectTwoSize.x();
	float leftEdge2 = rect2.xMin;
	float topEdge2 = rect2.yMin + rectTwoSize.y();
	float bottomEdge2 = rect2.yMin;

	bool xIsOverlapping = ValueInRange(leftEdge2, rightEdge2, leftEdge1)||
		ValueInRange(leftEdge1, rightEdge1,leftEdge2);

	bool yIsOverlapping = ValueInRange(bottomEdge1,topEdge1,bottomEdge2)||
		ValueInRange(bottomEdge2,topEdge2,bottomEdge1);

	return xIsOverlapping && yIsOverlapping;
}

RectA::RectA(float xMinp, float yMinp, float widthp, float heightp)
{
	xMin = xMinp;
	yMin = yMinp;
	width = widthp;
	height = heightp;
}

RectA::RectA()
{
	yMin = 0.f;
	width = 0.f;
	height = 0.f;
	xMin = 0.f;
}
