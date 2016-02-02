#include "NetPacket.hpp"

NetPacket::NetPacket(void *data, size_t data_len, sockaddr* saddr, size_t saddrlen)
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
	WriteBytes(data, data_len);
	address.Init(saddr);
}

NetPacket::NetPacket()
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
}

NetAddress const* NetPacket::GetAddress() const
{
	return &address;
}

uint32_t* NetPacket::GetBuffer()
{
	return m_buffer;
}

void ByteBuffer::Init(void* buffer, size_t max_size)
{
	m_maxSize = max_size;
	m_buffer = (uint32_t*)buffer;
	m_writeIndex = 0;
}

bool ByteBuffer::WriteBytes(void *data, size_t size)
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
