#include "NetPacket.hpp"
#include "NetMessage.hpp"
#include <string>

bool NetPacket::AddMessage(const NetMessage& msg)
{
	size_t msgLen = msg.GetLength();
	if (msgLen > BytesRemaining()) {
		return false;
	}

	Write<uint16_t>((uint16_t)msgLen + 3); 
	Write<uint8_t>((uint8_t)(msg.m_messageDefinition->m_id));

	WriteBytes(msg.m_buffer, msg.GetLength());
	++m_msgCount;

	return true;
}

void NetPacket::CreateHeader()
{
	unsigned short packetAckID = 0xffff;
	WriteBytes(&packetAckID, sizeof(unsigned short));
	UpdateNumMessages();
	m_numBytesWritten++;
}

void NetPacket::UpdateNumMessages()
{
	m_buffer[2] = (uint8_t)m_msgCount;
}

NetPacket::NetPacket(void *data, size_t data_len, sockaddr* saddr) :
	m_msgCount(0)
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
	WriteBytes(data, data_len);
	m_address.Init(saddr);
}

NetPacket::NetPacket() : 
	ByteBuffer(),
	m_msgCount(0)
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
	CreateHeader();
}

NetAddress* NetPacket::GetAddress()
{
	return &m_address;
}

unsigned char* NetPacket::GetBuffer()
{
	return m_buffer;
}

size_t NetPacket::BytesRemaining()
{
	return m_maxSize - m_numBytesWritten;
}



ByteBuffer::ByteBuffer()
{
	ByteBuffer::Init(m_buffer, PACKET_MTU);
}

void ByteBuffer::Init(void* buffer, size_t max_size)
{
	memset(buffer, 0, max_size);
	m_maxSize = max_size;
	m_numBytesWritten = 0;
	m_numBytesRead = 0;
}

bool ByteBuffer::WriteBytes(const void *data, size_t size, bool advanceBuffer/*=true*/)
{
	if ((m_numBytesWritten + size) > m_maxSize)
		return false;

	void* test = m_buffer + m_numBytesWritten;
	memcpy(test, data, size);

	m_numBytesWritten += size;

	//std::string dataAsString = std::string((char*)m_buffer);
	return true;
}

size_t ByteBuffer::GetLength() const
{
	return m_numBytesWritten;
}

unsigned char* ByteBuffer::GetBuffer()
{
	m_buffer[m_numBytesWritten] = '\0';
	return m_buffer;
}

void ByteBuffer::SetLength(size_t len)
{
	FATAL_ASSERT(len <= m_maxSize);
	m_numBytesWritten = len;
}

