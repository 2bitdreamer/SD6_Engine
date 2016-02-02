#pragma once
#include "Math/raw_vector.hpp"
#include <string>
#include <vector>
#include "Math/Matrices.h"

class BinaryBufferBuilder
{
public:
	BinaryBufferBuilder(size_t bufferSize);
	~BinaryBufferBuilder();

	void ResetBuffer();
	void WriteDouble(const double f);
	void WriteFloat(const float f);
	void FlushBufferToFile(const std::string& filePath, bool refreshBuffer=false);
	void WriteInt(const int i);
	void WriteUnsignedInt(const unsigned int i);
	void WriteShort(const short s);
	void WriteUnsignedShort(const unsigned short s);
	void WriteVec2(const Vec2& outVec2);
	void WriteVec3(const Vec3& outVec3);
	void WriteVec4(const Vec4& outVec4);
	void WriteMatrix4(const Matrix4& matrix4);
	void CreateDirectoriesForFilePath(const std::string& filePath);
	void WriteUnsignedChar(const unsigned char outChar);
	void WriteString(const std::string& str);
	void WriteString(const std::string& str, std::size_t size);

public:
	std::vector<unsigned char> m_charsToWrite;
	size_t m_bufferSize;
};


