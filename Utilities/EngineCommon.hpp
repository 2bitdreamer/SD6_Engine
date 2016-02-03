#pragma once
#define _USE_MATH_DEFINES


//#include "Engine/Memory.hpp"
//#define new new (__FILE__, __LINE__)

#ifdef _WIN32
#include <io.h>
#endif

#include <cassert>
#include <iostream>
#include <math.h>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/raw_vector.hpp"
#include "Engine/Utilities/Win32Wrapper.hpp"
#include <time.h>
#include "../Math/Vertex.hpp"

#include <winsock2.h>
struct addrinfo;
#define nothrow void
class NetAddress;



extern Win32Wrapper myWinWrapper;

extern bool g_isQuitting;
#pragma warning(disable:4996)  //'_splitpath': This function or variable may be unsafe. Consider using _splitpath_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.

const double TWO_PI = 2* M_PI;
const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 900;
//typedef unsigned char uint32_t;
typedef std::mt19937 RngT;
static time_t oldseed;
static RngT g_rng;
//static RngT g_rng((unsigned int)0);

//----------------------------------------------------------------------------------------------


struct KeyState {
	bool m_isPressed;
	bool m_hasKeyJustBeenReleased;
	KeyState() :
		m_isPressed(false),
		m_hasKeyJustBeenReleased(false) {}
};

struct MouseState {
	Vec2i m_mousePosition;

	bool m_isLeftButtonPressed;
	bool m_isRightButtonPressed;

	bool m_hasLeftButtonJustBeenReleased;
	bool m_hasRightButtonJustBeenReleased;

	MouseState() :
		m_hasRightButtonJustBeenReleased(false),
		m_isLeftButtonPressed(false),
		m_isRightButtonPressed(false),
		m_hasLeftButtonJustBeenReleased(false) {}
};

struct TextLine {
	TextLine::TextLine() :
		m_color(RGBA(0, 0, 100, 255)),
		m_textSize(50.f)
	{

	}

	Vec2 m_messageBeginPosition;
	RGBA m_color;
	std::string m_contents;
	float m_textSize;
};

// TYPES ////////////////////////////////////////////////////////////////////
typedef bool(*address_work_cb)(addrinfo*, void *user_arg);


void SockAddrFromNetAddr(sockaddr *addr, size_t *addrlen, NetAddress const &net_addr);

void* GetInAddr(sockaddr const *sa);

// FUNCTION PROTOTYPES //////////////////////////////////////////////////////
addrinfo* AllocAddressesForHost(char const *host,
	char const *service,
	int family,
	int socktype,
	bool binding);

void FreeAddresses(addrinfo* addresses);

uint16_t GetAddressPort(sockaddr const *addr);
size_t GetAddressName(char *buffer, size_t const buffer_size, sockaddr const *sa);

void ForEachAddress(addrinfo *addresses, address_work_cb cb, void *user_arg);

struct RectA {
	float xMin, yMin, width, height;
	RectA(float xMinp, float yMinp, float widthp, float heightp);
	RectA();
};

enum Direction
{
	EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, NORTH, NORTHEAST, NUM_DIRECTIONS
};

const std::string Stringf( const char* format, ... );

const std::string Stringf( const int maxLength, const char* format, ... );
template<typename T> bool ValueInRange(const T& start, const T& end, const T& value);
std::vector<std::string> SplitString(const std::string& input, const std::string& delimiter);
RGBA RandomColor();

void DebuggerPrintf( const char* messageFormat, ... );

size_t GetFileLength(const std::string& filePath);
size_t GetFileLength(FILE* openFile);
void GetFilesList(const std::string& dir, std::vector<std::string>& foundFilesOut);
void FindAllFilesOfType(const std::string& directory, const std::string& searchPattern, std::vector<std::string>& out_filesFound);
unsigned char StringToUnsignedChar(const std::string& c);
RGBA ExtractRGBA(const std::string& attribute);

Vec2i ExtractVec2i(const std::string& attribute);
std::string RGBAtoString(const RGBA& rgba);
std::string Vec3ToString(const Vec3& pos);

std::string Vec2iToString(const Vec2i& pos);
int Distance(int x1, int x2, int y1, int y2);
void GetFilesRecursively(const std::string& dir, std::vector<std::string>& foundFilesOut);
void GenerateFiles(int numFiles, size_t fileSize, const std::string& basePath);
size_t HashBytes(const std::vector<unsigned char>& data);
bool LoadFileToNewBinaryBuffer(const std::string& filePath, unsigned char* &out_binaryBuffer, size_t& out_bufferSize);
bool LoadFileToVector(const std::string& filePath, std::vector< unsigned char >&out_dataVector);
bool LoadFileToExistingBinaryBuffer(const std::string& filePath, char* &out_textBuffer, size_t bufferSizeLimit);
bool LoadFileToNewTextBuffer(const std::string& filePath, char* &out_textBuffer);
bool LoadFileToExistingString(const std::string& filePath, std::string& out_string);

bool FlushTextBufferToDisk(const std::string& filePath, const char* textDataBuffer);
bool FlushByteBufferToDisk(const std::string& filePath, const unsigned char* byteBuffer, size_t bufferSize);
bool FlushByteVectorToDisk(const std::string& filePath, const std::vector< unsigned char >& byteVector);
bool FlushStringToDisk(const std::string& filePath, const std::string& stringToWrite);



//void ReportAllocations();


float DegreesToRadians(float deg);
bool IsLittleEndian();
void SwitchToFixedFunctionPipeline();
float RandomAngle();
Vec3 Vec2iToVec3(const Vec2i& inVec2i);
std::vector<Vertex> SquareFromPoint(const Vec3& bottomLeftPoint, float size, const RGBA& color, const Vec3& insetVector);
float RandomFloat(float min, float max);
unsigned char RandomUnsignedChar(unsigned char min, unsigned char max);
int RandomInt(int min, int max, RngT& rng);
int RandomInt(int min, int max);

inline Direction GetRandomCardinalDirection(RngT& rng)
{
	return Direction(std::uniform_int_distribution<int>(0, 3)(rng));
}

inline Direction GetRandomCardinalDirection()
{
	int randomCardinalDir = RandomInt(0, 3);
	switch (randomCardinalDir) {
	case 0:
		return EAST;
		break;
	case 1:
		return WEST;
		break;
	case 2:
		return SOUTH;
		break;
	case 3:
		return NORTH;
		break;
	default:
		return EAST;
	}
}

inline Direction GetRandomDirection()
{
	return (Direction)RandomInt(0, NUM_DIRECTIONS);
}

bool IsWhitespace(char c);
char* Trim(char *s);

// COMMENT: Code duplication  (Nothing to do with this project. For a specific class
//Christian will look at it later.)
inline std::string GetDiffuseTexturePathForModelName(const std::string& modelToLoad) {
	std::string dataPath = "Data/Models/" + modelToLoad + "/" + modelToLoad;
	std::string texturePath = dataPath;
	texturePath += "_Diffuse";
	texturePath += ".png";
	return texturePath;
}

inline std::string GetNormalsTexturePathForModelName(const std::string& modelToLoad) {
	std::string dataPath = "Data/Models/" + modelToLoad + "/" + modelToLoad;
	std::string texturePath = dataPath;
	texturePath += "_Normals";
	texturePath += ".png";
	return texturePath;
}

inline std::string GetSpecularTexturePathForModelName(const std::string& modelToLoad) {
	std::string dataPath = "Data/Models/" + modelToLoad + "/" + modelToLoad;
	std::string texturePath = dataPath;
	texturePath += "_SpecGlossEmit";
	texturePath += ".png";
	return texturePath;
}


bool StringEndsWith(const std::string& value, const std::string& ending);
bool StringBeginsWith(const std::string& value, const std::string& beginning);
bool StringReplace(std::string& str, const std::string& from, const std::string& to);
void StringReplaceAll(std::string& str, const std::string& from, const std::string& to);
bool DoesRectOverlapRect(const Vec2& rectOnePos, const Vec2& rectOneSize, const Vec2& rectTwoPos, const Vec2& rectTwoSize);

template<typename T, typename U> U select2nd_f(const std::pair<T, U>& p) { return p.second; }
bool DoesRectOverlapRect(const RectA& rect1, const RectA& rect2);