#include "NetPacket.hpp"
#include "NetMessage.hpp"

bool NetPacket::AddMessage(const NetMessage& msg)
{
	size_t msgLen = msg.GetRequiredSpaceInPacket();
	if (msgLen > BytesRemaining()) {
		return false;
	}

	Write<uint16_t>((uint16_t)msgLen);
	Write<uint8_t>((uint8_t)(msg.m_messageDefinition->m_id));

	WriteBytes((void*)msg.m_buffer, msg.GetLength());
	++m_msgCount;

}

NetPacket::NetPacket(void *data, size_t data_len, sockaddr* saddr) :
	m_msgCount(0)
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
	WriteBytes(data, data_len);
	address.Init(saddr);
}

NetPacket::NetPacket() :
	m_msgCount(0)
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
}

const NetAddress* NetPacket::GetAddress() const
{
	return &address;
}

uint32_t* NetPacket::GetBuffer()
{
	return m_buffer;
}

size_t NetPacket::BytesRemaining()
{
	return (m_maxSize - (m_writeIndex + 3));
}

void ByteBuffer::Init(void* buffer, size_t max_size)
{
	m_maxSize = max_size;
	m_buffer = (uint32_t*)buffer;
	m_writeIndex = 0;
}

bool ByteBuffer::WriteBytes(const void *data, size_t size)
{
	if ((m_maxSize + size) > PACKET_MTU)
		return false;

	memcpy(m_buffer + m_writeIndex, data, size);
	m_writeIndex += size;
	m_maxSize += size;

	return true;
}

size_t ByteBuffer::GetLength() const
{
	return m_writeIndex;
}

uint32_t* ByteBuffer::GetBuffer()
{
	return m_buffer;
}

void ByteBuffer::SetLength(size_t len)
{
	FATAL_ASSERT(len <= m_maxSize);
	m_writeIndex = len;
}

template<typename T>
bool ByteBuffer::Write(const T& v)
{
	return WriteBytes(&v, sizeof(T));
}

template<typename T>
bool ByteBuffer::Read(const T& v)
{
	return (ReadBytes(&v, sizeof(T)) == sizeof(T));
}
