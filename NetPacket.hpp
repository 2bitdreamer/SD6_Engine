#pragma once
#include "Assert.hpp"
#include "NetAddress.hpp"
#include "Utilities\EngineCommon.hpp"
class NetMessage;

class ByteBuffer;
size_t const PACKET_MTU = 1400; // Maximum Packet Size

								// Manages writing and reading messages
								// and contains information on who it's from/who it's going to.

class ByteBuffer
{
public:
	unsigned char m_buffer[PACKET_MTU];
	size_t m_maxSize;

	size_t m_numBytesWritten;
	size_t m_numBytesRead;

	ByteBuffer();
	void Init(void* buffer, size_t max_size);

	// When you write, you write to the end of the buffer
	// @return 
	//    true:  Was enough room and was written
	//    false: Not enough room, and was not written.
	bool WriteBytes(const void *data, size_t size);

	// Reads from buffer, copying data up to size into data
	// @return 
	//    number of bytes read, should equal size on success
	//    If less than size, there was not enough data.  
	//    Advanced read index either way.
	size_t ReadBytes(void *out_data, size_t size);
	size_t GetLength() const;
	unsigned char* GetBuffer();
	void SetLength(size_t len);

	template<typename T>
	bool Write(const T& v)
	{
		return WriteBytes(&v, sizeof(T));
	}

	template<typename T>
	bool Read(const T& v)
	{
		return (ReadBytes(&v, sizeof(T)) == sizeof(T));
	}
};

class NetPacket : public ByteBuffer
{
public:
	NetAddress m_address; // to or from, depending on usage

	bool AddMessage(const NetMessage& msg);

	void CreateHeader();
	void UpdateNumMessages();

	NetPacket();
	NetPacket(void *data, size_t data_len, sockaddr* saddr);
	NetAddress* GetAddress();
	unsigned char* GetBuffer();
	size_t BytesRemaining();

	size_t m_msgCount;
};

