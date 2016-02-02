#pragma once
#include <vector>
#include "..\Math\raw_vector.hpp"
#include <string>
#include "..\Math\Matrices.h"
#include "..\Math\Vertex.hpp"
#include <vector>

class BinaryBufferParser {
public:
	BinaryBufferParser(const unsigned char* byteBuffer, size_t dataSize);
	BinaryBufferParser(const std::vector< unsigned char >& bytes);
	BinaryBufferParser();

	bool WillObjectOfSizeFit(size_t size);

	const float ReadFloat();
	bool ReadFloat(float& out_float);

	const int ReadInt();
	bool ReadInt(int& out_int);

	const short ReadShort();
	bool ReadShort(short& out_short);

	bool ReadCharArray(std::vector<char>& chars, unsigned int numChars);
	std::vector<char> ReadCharArray(unsigned int numChars);

	const unsigned int ReadUnsignedInt();
	bool ReadUnsignedInt(unsigned int& out_uint);

	const unsigned short ReadUnsignedShort();
	bool ReadUnsignedShort(unsigned short& out_ushort);

	bool ReadC23(std::vector<Vertex>& out_vertexList);

	char ReadChar();
	bool ReadChar(char& out_char);

	const unsigned char ReadUnsignedChar();
	bool ReadUnsignedChar(unsigned char& out_uchar);

	const double ReadDouble();
	bool ReadDouble(double& out_double);

	const std::string ReadString();
	bool ReadString(std::string& out_string);

	const bool ReadBool();
	bool ReadBool(bool& out_bool);

	const Vec2 ReadVec2();
	bool ReadVec2(Vec2& out_vec2);

	const Vec3 ReadVec3();
	bool ReadVec3(Vec3& out_vec3);

	const Vec4 ReadVec4();
	bool ReadVec4(Vec4& out_vec4);

	bool ReadRGBA(RGBA& out_RGBA);
	const RGBA ReadRGBA();

	const Matrix4 ReadMatrix4();
	bool ReadMatrix4(Matrix4& out_mat4);

	template <typename T> bool ReadPOD(T& out_data);
	template <typename T> bool ReadPODList(std::vector<T>& out_data, unsigned int numElements);

	const bool IsAtBufferEnd();

	void RewindBuffer();
public:
	size_t m_bufferSize;
	const unsigned char* m_currentReadPosition;
	const unsigned char* m_byteBuffer;
};

template <typename T>
bool BinaryBufferParser::ReadPOD(T& out_data) {
	const size_t Tsize = sizeof(T);
	if (!WillObjectOfSizeFit(Tsize))
		return false;

	out_data = *((T*)m_currentReadPosition);

	m_currentReadPosition += Tsize;
	return true;
}

template <typename T>
bool BinaryBufferParser::ReadPODList(std::vector<T>& out_data, unsigned int numElements) {
	bool anyElementFailedRead = false;
	out_data.resize(numElements);
	for (size_t index = 0; index < numElements; index++) {
		T outputElement;
		if (!ReadPOD(outputElement))
			anyElementFailedRead = true;

		out_data[index] = outputElement;
	}
	return !anyElementFailedRead;
}



