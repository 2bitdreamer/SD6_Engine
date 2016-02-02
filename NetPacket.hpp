#pragma once
#include "Assert.hpp"
#include "NetAddress.hpp"
#include "Utilities\EngineCommon.hpp"

class ByteBuffer;
size_t const PACKET_MTU = 1400; // Maximum Packet Size

								// Manages writing and reading messages
								// and contains information on who it's from/who it's going to.

class ByteBuffer
{
public:
	uint32_t* m_buffer;
	size_t m_maxSize;

	size_t m_writeIndex;
	size_t m_readIndex;

	void Init(void* buffer, size_t max_size);

	// When you write, you write to the end of the buffer
	// @return 
	//    true:  Was enough room and was written
	//    false: Not enough room, and was not written.
	bool WriteBytes(void *data, size_t size);

	// Reads from buffer, copying data up to size into data
	// @return 
	//    number of bytes read, should equal size on success
	//    If less than size, there was not enough data.  
	//    Advanced read index either way.
	size_t ReadBytes(void *out_data, size_t size);
	size_t GetLength() const;
	uint32_t* GetBuffer();
	void SetLength(size_t len);

	template<typename T>
	bool Write(T const &v)
	{
		WriteBytes(&v, sizeof(Tx));
	}

	template<typename T>
	bool Read(T const &v)
	{
		return (ReadBytes(&v, sizeof(T)) == sizeof(T));
	}
};

class NetPacket : public ByteBuffer
{
public:
	uint32_t m_buffer[PACKET_MTU];
	NetAddress address; // to or from, depending on usage

	NetPacket();
	NetPacket(void *data, size_t data_len, sockaddr* saddr, size_t saddrlen);
	NetAddress const* GetAddress() const;
	uint32_t* GetBuffer();


};

