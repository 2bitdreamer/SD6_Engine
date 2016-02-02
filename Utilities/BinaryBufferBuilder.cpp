#include "BinaryBufferBuilder.hpp"
#include <windows.h>


BinaryBufferBuilder::BinaryBufferBuilder(size_t bufferSize)
{
	m_charsToWrite.reserve(bufferSize);
}


BinaryBufferBuilder::~BinaryBufferBuilder()
{
}


void BinaryBufferBuilder::WriteDouble(const double d)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&d), (unsigned char*)(&d + 1));
}

void BinaryBufferBuilder::WriteFloat(const float f)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&f), (unsigned char*)(&f + 1));
}

void BinaryBufferBuilder::WriteInt(const int i)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&i), (unsigned char*)(&i + 1));
}

void BinaryBufferBuilder::WriteUnsignedInt(const unsigned int i)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&i), (unsigned char*)(&i + 1));
}

void BinaryBufferBuilder::WriteShort(const short s)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&s), (unsigned char*)(&s + 1));
}

void BinaryBufferBuilder::WriteBool(const bool b)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&b), (unsigned char*)(&b + 1));
}

void BinaryBufferBuilder::WriteUnsignedShort(const unsigned short s)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&s), (unsigned char*)(&s + 1));
}

void BinaryBufferBuilder::WriteChar(const char c)
{
	m_charsToWrite.insert(m_charsToWrite.end(), (unsigned char*)(&c), (unsigned char*)(&c + 1));
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

void BinaryBufferBuilder::CreateDirectoriesForFilePath(const std::string& filePath)
{
	CreateDirectory((LPCWSTR)filePath.c_str(), NULL);
}

void BinaryBufferBuilder::ResetBuffer() {
	m_charsToWrite.resize(0);
}


void BinaryBufferBuilder::FlushBufferToFile(const std::string& filePath, bool refreshBuffer/*=false*/)
{
	FILE* file;
	fopen_s(&file, filePath.c_str(), "wb+");

	if (!file)
	{
		CreateDirectoriesForFilePath(filePath);
	}

	if (file)
		fwrite(m_charsToWrite.data(), 1, m_charsToWrite.size(), file);

	if (refreshBuffer)
		ResetBuffer();

	fclose(file);

}


