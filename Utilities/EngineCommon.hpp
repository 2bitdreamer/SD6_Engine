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
#include "../NamedProperties.hpp"
#include "../Assert.hpp"
#define nothrow void

typedef void (EventCallback)(NamedProperties& args);


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


template<typename T_ObjType, typename T_MethodPtrType>
void RegisterEventSubscriber(const std::string& eventName, T_ObjType& obj, T_MethodPtrType method) {
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.RegisterEventSubscription(eventName, obj, method);
}

template <typename T_ObjType, typename T_MethodPtrType>
void  UnregisterObjMethodForEvent(const std::string& eventName, T_ObjType& object, T_MethodPtrType method) {
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.UnregisterEventSubscription(eventName, object, method);
}

template <typename T_ObjType>
void  UnregisterEventForObject(const std::string& eventName, T_ObjType& object) {
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.UnregisterEventForObject(eventName, object);
}

template <typename T_ObjType>
void UnregisterObjectFromAllEvents(T_ObjType& object) {
	EventSystem& eventSys = EventSystem::GetInstance();
	eventSys.UnregisterObjectFromAllEvents(object);
}

class Object {
public:
	virtual ~Object();
	Object();
};

struct KeyState {
	bool m_isPressed;
	bool m_hasKeyJustBeenReleased;
	KeyState() :
		m_isPressed(false),
		m_hasKeyJustBeenReleased(false) {}
};

enum UIState {
	UI_STATE_DEFAULT,
	UI_STATE_HIGHLIGHTED,
	UI_STATE_PRESSED,
	UI_STATE_HIDDEN,
	UI_STATE_DISABLED,
	UI_STATE_ALL,
	NUM_UI_STATES
};

enum MouseEventType {
	LEFT_BUTTON_DOWN,
	LEFT_BUTTON_UP,
	RIGHT_BUTTON_DOWN,
	RIGHT_BUTTON_UP,
	MOVED,
	NONE
};

struct MouseEvent {
	Vec2 m_cursorPos;
	MouseEventType m_mouseEventType;
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

struct RectA {
	float xMin, yMin, width, height;
	RectA(float xMinp, float yMinp, float widthp, float heightp);
	RectA();
};

enum Direction
{
	EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST, NORTH, NORTHEAST, NUM_DIRECTIONS
};

/////////////////////////////////////////////////////////////////////////////////////////////////
enum CardinalDir
{
	C_DIRECTION_NONE = 0,
	C_DIRECTION_EAST = 1,
	C_DIRECTION_NORTH = 2,
	C_DIRECTION_WEST = 3,
	C_DIRECTION_SOUTH = 4,
	NUM_C_DIRECTIONS
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


const std::string Stringf( const char* format, ... );

const std::string Stringf( const int maxLength, const char* format, ... );
template<typename T> bool ValueInRange(const T& start, const T& end, const T& value);
std::vector<std::string> SplitString(const std::string& input, const std::string& delimiter);
RGBA RandomColor();

void DebuggerPrintf( const char* messageFormat, ... );

size_t GetFileLength(const std::string& filePath);
size_t GetFileLength(FILE* openFile);
void GetFilesList(const std::string& dir, std::vector<std::string>& foundFilesOut);
void FireEventForEachFileFound(const std::string& eventToFire, const std::string& baseFolder, const std::string& filePattern, bool recurseSubfolders);
void EnumerateFiles(const std::string& dir, const std::string& filePattern, std::vector<std::string>& foundFilesOut, bool recurseFiles/*=true*/);
void FindAllFilesOfType(const std::string& directory, const std::string& searchPattern, std::vector<std::string>& out_filesFound);
unsigned char StringToUnsignedChar(const std::string& c);
RGBA ExtractRGBA(const std::string& attribute);

Vec2i ExtractVec2i(const std::string& attribute);
std::string RGBAtoString(const RGBA& rgba);
std::string Vec3ToString(const Vec3& pos);

std::string Vec2iToString(const Vec2i& pos);
int Distance(int x1, int x2, int y1, int y2);
void GetFilesRecursively(const std::string& dir, const std::string& filePattern, std::vector<std::string>& foundFilesOut);
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

void FireEvent(const std::string& eventName, NamedProperties& arg, bool fromConsole = false);
void FireEvent(const std::string& eventName, bool fromConsole = false);

void RegisterEventCallback(const std::string& eventName, EventCallback* callbackFunc);
void UnregisterEventCallback(const std::string& eventName, EventCallback* callbackFunc);

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
//I will look at it later.)
inline std::string GetDiffuseTexturePathForModelName(const std::string& modelToLoad) {
	std::string dataPath = "Data/Models/" + modelToLoad + "/" + modelToLoad;
	std::string texturePath = dataPath;
	texturePath += "_Diffuse";
	texturePath += ".png";
	return texturePath;
}


inline int Interpolate(const int& start, const int& end, float fractionComplete) {
	int numSlots = 1 + end - start;
	float parametricPerSlot = 1.f / (float)numSlots;
	int lerpedSlot = (int)floorf(fractionComplete / parametricPerSlot);
	return start + lerpedSlot;
}

inline unsigned char Interpolate(const unsigned char& start, const unsigned char& end, float fractionComplete) {
	return (unsigned char)Interpolate((int)start, (int)end, fractionComplete);
}

inline RGBA Interpolate(const RGBA& start, const RGBA& end, float fractionComplete) {
	RGBA interpRGBA;
	interpRGBA.r() = Interpolate(start.r(), end.r(), fractionComplete);
	interpRGBA.g() = Interpolate(start.g(), end.g(), fractionComplete);
	interpRGBA.b() = Interpolate(start.b(), end.b(), fractionComplete);
	interpRGBA.a() = Interpolate(start.a(), end.a(), fractionComplete);
	return interpRGBA;
}



inline std::string Interpolate(const std::string& start, const std::string& end, float fractionComplete) {
	size_t startSize = start.size();
	size_t endSize = end.size();

	float fractionStart = 1.f - fractionComplete;
	size_t startIndex = (size_t)(startSize * fractionStart);
	size_t endIndex = (size_t)(endSize * fractionComplete);

	std::string startString = start.substr(0, startIndex);
	std::string endString = end.substr(0, endIndex);

	return (startString + endString);
}

template <typename T>
inline T Interpolate(const T& start, const T& end, float fractionComplete) {
	return (T)(start * (1.f - fractionComplete)) + (end * fractionComplete);
}


inline CardinalDir Interpolate(const CardinalDir& start, const CardinalDir& end, float fractionComplete) {
	(void)end;
	(void)fractionComplete;
	return start;
}

inline std::vector<std::string> Interpolate(const std::vector<std::string>& start, const std::vector<std::string>& end, float fractionComplete) {
	(void)fractionComplete;
	if (!start.empty())
		return start;
	else
		return end;
}


template <>
inline bool Interpolate<bool>(const bool& start, const bool& end, float fractionComplete) {
	(void)start;
	(void)end;
	if (fractionComplete >= 0.5f)
		return true;
	else
		return false;
}

template <typename T>
inline T Clamp(T x, T low, T high)
{
	return x < low ? low : (x > high ? high : x);

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