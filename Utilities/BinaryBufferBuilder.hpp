#pragma once
#include "Engine/Math/raw_vector.hpp"
#include <string>
#include <vector>
#include "Engine/Math/Matrices.h"

class BinaryBufferBuilder
{
public:
	BinaryBufferBuilder(size_t bufferSize);
	BinaryBufferBuilder();
	~BinaryBufferBuilder();

	void ResetBuffer();

	void WriteDouble(const double f);
	void WriteFloat(const float f);
	void WriteInt(const int i);
	void WriteUnsignedInt(const unsigned int i);
	void WriteShort(const short s);
	void WriteBool(const bool b);
	void WriteUnsignedShort(const unsigned short s);
	void WriteChar(const char c);
	void WriteVec2(const Vec2& outVec2);
	void WriteVec3(const Vec3& outVec3);
	void WriteVec4(const Vec4& outVec4);
	void WriteMatrix4(const Matrix4& matrix4);
	void WriteUnsignedChar(const unsigned char outChar);
	void WriteString(const std::string& str);
	void WriteString(const std::string& str, std::size_t size);

	void FlushBufferToFile(const std::string& filePath, bool refreshBuffer = false);
	void CreateDirectoriesForFilePath(const std::string& filePath);

public:
	std::vector<unsigned char> m_charsToWrite;
	//size_t m_bufferSize;
};




