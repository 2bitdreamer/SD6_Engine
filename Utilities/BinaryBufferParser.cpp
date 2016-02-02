#include "BinaryBufferParser.hpp"
#include <algorithm>
#include <assert.h>

BinaryBufferParser::BinaryBufferParser(const unsigned char* byteBuffer, size_t dataSize) :
m_currentReadPosition(byteBuffer),
m_byteBuffer(byteBuffer),
m_bufferSize(dataSize)
{
}

BinaryBufferParser::BinaryBufferParser(const std::vector< unsigned char >& bytes) :
m_currentReadPosition(&bytes.front()),
m_byteBuffer(&bytes.front()),
m_bufferSize(bytes.size())
{
}

BinaryBufferParser::BinaryBufferParser() :
m_bufferSize(0),
m_currentReadPosition(nullptr),
m_byteBuffer(nullptr)
{

}

const double BinaryBufferParser::ReadDouble() {

	double parsedDouble;
	ReadDouble(parsedDouble);
	return parsedDouble;
}

bool BinaryBufferParser::ReadDouble(double& out_double) {
	const int doubleSize = sizeof(double);
	if (!WillObjectOfSizeFit(doubleSize))
		return false;

	out_double = *((double*)m_currentReadPosition);

	m_currentReadPosition += doubleSize;
	return true;
}

const float BinaryBufferParser::ReadFloat()
{
	float parsedFloat;
	ReadFloat(parsedFloat);
	return parsedFloat;
}

bool BinaryBufferParser::WillObjectOfSizeFit(size_t size) {
	const unsigned char* endPosition = m_byteBuffer + m_bufferSize;
	bool objectFits = m_currentReadPosition + size <= endPosition;
	return objectFits;
}


bool BinaryBufferParser::ReadFloat(float& out_float)
{
	const size_t floatSize = sizeof(float);
	if (!WillObjectOfSizeFit(floatSize))
		return false;

	out_float = *((float*)m_currentReadPosition);

	m_currentReadPosition += floatSize;
	return true;
}

const int BinaryBufferParser::ReadInt()
{
	int parsedInt;
	ReadInt(parsedInt);
	return parsedInt;
}

bool BinaryBufferParser::ReadInt(int& out_int)
{
	const size_t intSize = sizeof(int);
	if (!WillObjectOfSizeFit(intSize))
		return false;

	out_int = *((int*)m_currentReadPosition);

	m_currentReadPosition += intSize;
	return true;
}


const unsigned int BinaryBufferParser::ReadUnsignedInt()
{
	unsigned int parsedUnsignedInt;
	ReadUnsignedInt(parsedUnsignedInt);
	return parsedUnsignedInt;
}

bool BinaryBufferParser::ReadUnsignedInt(unsigned int& out_uint)
{
	const size_t unsignedIntSize = sizeof(unsigned int);
	if (!WillObjectOfSizeFit(unsignedIntSize))
		return false;

	out_uint = *((unsigned int*)m_currentReadPosition);

	m_currentReadPosition += unsignedIntSize;
	return true;
}

const short BinaryBufferParser::ReadShort()
{
	short parsedShort;
	ReadShort(parsedShort);
	return parsedShort;
}

bool BinaryBufferParser::ReadShort(short& out_short)
{
	const size_t shortSize = sizeof(short);
	if (!WillObjectOfSizeFit(shortSize))
		return false;

	out_short = *((short*)m_currentReadPosition);

	m_currentReadPosition += shortSize;
	return true;

}

bool BinaryBufferParser::ReadCharArray(std::vector<char>& chars, unsigned int numChars) {
	return ReadPODList(chars, numChars);
}

std::vector<char> BinaryBufferParser::ReadCharArray(unsigned int numChars)
{
	std::vector<char> chars(numChars);
	ReadPODList(chars, numChars);
	return chars;
}


const unsigned short BinaryBufferParser::ReadUnsignedShort()
{
	unsigned short parsedUnsignedShort;
	ReadUnsignedShort(parsedUnsignedShort);
	return parsedUnsignedShort;
}

bool BinaryBufferParser::ReadUnsignedShort(unsigned short& out_ushort)
{
	const size_t unsignedShortSize = sizeof(unsigned short);
	if (!WillObjectOfSizeFit(unsignedShortSize))
		return false;

	out_ushort = *((short*)m_currentReadPosition);

	m_currentReadPosition += unsignedShortSize;
	return true;
}

const unsigned char BinaryBufferParser::ReadUnsignedChar()
{
	unsigned char parsedUnsignedChar;
	ReadUnsignedChar(parsedUnsignedChar);
	return parsedUnsignedChar;
}


char BinaryBufferParser::ReadChar() {
	char resultChar;
	ReadChar(resultChar);
	return resultChar;
}

bool BinaryBufferParser::ReadChar(char& out_char) {
	const size_t charSize = sizeof(char);
	if (!WillObjectOfSizeFit(charSize))
		return false;

	out_char = *((char*)m_currentReadPosition);

	m_currentReadPosition += charSize;
	return true;
}




bool BinaryBufferParser::ReadUnsignedChar(unsigned char& out_uchar)
{
	const size_t unsignedCharSize = sizeof(unsigned char);
	if (!WillObjectOfSizeFit(unsignedCharSize))
		return false;

	out_uchar = *((unsigned char*)m_currentReadPosition);

	m_currentReadPosition += unsignedCharSize;
	return true;
}



const std::string BinaryBufferParser::ReadString()
{
	std::string parsedString;
	ReadString(parsedString);
	return parsedString;
}

bool BinaryBufferParser::ReadString(std::string& out_string)
{
	size_t stringLength = strlen((const char*)m_currentReadPosition);
	if (!WillObjectOfSizeFit(stringLength))
		return false;

	out_string = std::string((const char*)m_currentReadPosition, stringLength);
	int numBytesToAdvance = stringLength + 1;
	m_currentReadPosition += numBytesToAdvance;

	return true;
}

const bool BinaryBufferParser::ReadBool()
{
	bool parsedBool;
	ReadBool(parsedBool);
	return parsedBool;
}

bool BinaryBufferParser::ReadBool(bool& out_bool)
{
	const size_t boolSize = sizeof(bool);
	if (!WillObjectOfSizeFit(boolSize))
		return false;

	out_bool = *((bool*)m_currentReadPosition);

	m_currentReadPosition += boolSize;
	return true;
}


const Vec2 BinaryBufferParser::ReadVec2()
{
	Vec2 parsedVec2;
	ReadVec2(parsedVec2);
	return parsedVec2;
}

bool BinaryBufferParser::ReadVec2(Vec2& out_vec2)
{
	float x;
	bool xFloat = ReadFloat(x);

	float y;
	bool yFloat = ReadFloat(y);

	if (!(xFloat && yFloat))
		return false;

	out_vec2.x() = x;
	out_vec2.y() = y;

	return true;

}



const Vec3 BinaryBufferParser::ReadVec3()
{
	Vec3 parsedVec3;
	ReadVec3(parsedVec3);
	return parsedVec3;
}

bool BinaryBufferParser::ReadVec3(Vec3& out_vec3)
{

	float x;
	bool xFloat = ReadFloat(x);

	float y;
	bool yFloat = ReadFloat(y);

	float z;
	bool zFloat = ReadFloat(z);

	if (!(xFloat && yFloat && zFloat))
		return false;

	out_vec3.x() = x;
	out_vec3.y() = y;
	out_vec3.z() = z;

	return true;
}


const Vec4 BinaryBufferParser::ReadVec4()
{
	Vec4 parsedVec4;
	ReadVec4(parsedVec4);
	return parsedVec4;
}

bool BinaryBufferParser::ReadVec4(Vec4& out_vec4)
{

	float x;
	bool xFloat = ReadFloat(x);

	float y;
	bool yFloat = ReadFloat(y);

	float z;
	bool zFloat = ReadFloat(z);

	float w;
	bool wFloat = ReadFloat(w);

	if (!(xFloat && yFloat && zFloat && wFloat))
		return false;

	out_vec4.x() = x;
	out_vec4.y() = y;
	out_vec4.z() = z;
	out_vec4.w() = w;

	return true;
}


const RGBA BinaryBufferParser::ReadRGBA()
{
	RGBA parsedRGBA;
	ReadRGBA(parsedRGBA);
	return parsedRGBA;
}

bool BinaryBufferParser::ReadRGBA(RGBA& out_RGBA)
{

	unsigned char r;
	bool rUchar = ReadUnsignedChar(r);

	unsigned char g;
	bool gUchar = ReadUnsignedChar(g);

	unsigned char b;
	bool bUchar = ReadUnsignedChar(b);

	unsigned char a;
	bool aUchar = ReadUnsignedChar(a);

	if (!(rUchar && gUchar && bUchar && aUchar))
		return false;

	out_RGBA.r() = r;
	out_RGBA.g() = g;
	out_RGBA.b() = b;
	out_RGBA.a() = a;

	return true;
}

const Matrix4 BinaryBufferParser::ReadMatrix4() {
	Matrix4 parsedMatrix;
	ReadMatrix4(parsedMatrix);
	return parsedMatrix;
}

bool BinaryBufferParser::ReadMatrix4(Matrix4& out_mat4) {

	Vec4 firstRow;
	Vec4 secondRow; 
	Vec4 thirdRow;
	Vec4 fourthRow;

	bool fiv = ReadVec4(firstRow);
	bool sv = ReadVec4(secondRow);
	bool tv = ReadVec4(thirdRow);
	bool fov = ReadVec4(fourthRow);

	if (!(fiv && sv && tv && fov))
		return false;

	out_mat4.SetRow(0, firstRow);
	out_mat4.SetRow(1, secondRow);
	out_mat4.SetRow(2, thirdRow);
	out_mat4.SetRow(3, fourthRow);

	return true;
}

const bool BinaryBufferParser::IsAtBufferEnd()
{
	const unsigned char* endPosition = m_byteBuffer + m_bufferSize;
	return m_currentReadPosition >= endPosition;
}

void BinaryBufferParser::RewindBuffer()
{
	m_currentReadPosition = m_byteBuffer;
}



