#include "BinaryBufferBuilder.hpp"
#include <windows.h>


BinaryBufferBuilder::BinaryBufferBuilder(size_t bufferSize) :
m_bufferSize(bufferSize)
{
	m_charsToWrite.reserve(bufferSize);
}


BinaryBufferBuilder::~BinaryBufferBuilder()
{
}

void BinaryBufferBuilder::ResetBuffer() {
	m_charsToWrite.resize(0);
	m_charsToWrite.reserve(m_bufferSize);
}

void BinaryBufferBuilder::FlushBufferToFile(const std::string& filePath, bool refreshBuffer/*=false*/)
{
		std::string filePath = filePath;
		FILE* file = fopen(filePath.c_str(), "wb");

		if (!file)
		{
			CreateDirectoriesForFilePath(filePath);
		}

		if (file)
			fwrite(m_charsToWrite.data(), 1, m_charsToWrite.size(), file);

		if (refreshBuffer)
			ResetBuffer();
}

void BinaryBufferBuilder::WriteInt(const int i)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (char*)(&i), (char*)(&i + 1));
}

void BinaryBufferBuilder::WriteUnsignedInt(const unsigned int i)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (char*)(&i), (char*)(&i + 1));
}

void BinaryBufferBuilder::WriteShort(const short s)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (char*)(&s), (char*)(&s + 1));
}

void BinaryBufferBuilder::WriteUnsignedShort(const unsigned short s)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (char*)(&s), (char*)(&s + 1));
}

void BinaryBufferBuilder::WriteVec2(const Vec2& outVec2)
{
	WriteFloat(outVec2.x());
	WriteFloat(outVec2.y());
}

void BinaryBufferBuilder::WriteVec3(const Vec3& outVec3)
{
	WriteFloat(outVec3.x());
	WriteFloat(outVec3.y());
	WriteFloat(outVec3.z());
}

void BinaryBufferBuilder::WriteVec4(const Vec4& outVec4)
{
	WriteFloat(outVec4.x());
	WriteFloat(outVec4.y());
	WriteFloat(outVec4.z());
	WriteFloat(outVec4.w());
}

void BinaryBufferBuilder::WriteMatrix4(const Matrix4& mat4) {
	WriteVec4(mat4.GetRow(0));
	WriteVec4(mat4.GetRow(1));
	WriteVec4(mat4.GetRow(2));
	WriteVec4(mat4.GetRow(3));
}

void BinaryBufferBuilder::CreateDirectoriesForFilePath(const std::string& filePath)
{
	CreateDirectory((LPCWSTR)filePath.c_str(), NULL);
}

void BinaryBufferBuilder::WriteUnsignedChar(const unsigned char outChar) {
	m_charsToWrite.push_back(outChar);
}

void BinaryBufferBuilder::WriteString(const std::string& str, std::size_t size)
{
	m_charsToWrite.insert(m_charsToWrite.end(), str.begin(), str.begin() + size);
}

void BinaryBufferBuilder::WriteString(const std::string& str)
{
	m_charsToWrite.insert(m_charsToWrite.end(), str.begin(), str.end());
	m_charsToWrite.push_back('\0');
}
