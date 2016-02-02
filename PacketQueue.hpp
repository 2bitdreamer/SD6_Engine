#pragma once
class NetPacket;
struct sockaddr;

#include "Utilities\ThreadSafeQueue.hpp"
#include "Utilities\EngineCommon.hpp"

class NetPacketQueue
{
private:
	ThreadSafeQueue<NetPacket*> m_incoming;
	ThreadSafeQueue<NetPacket*> m_outgoing;

public:
	void EnqueueOutgoing(NetPacket *packet);
	void EnqueueOutgoing(uint32_t *buffer, size_t len, sockaddr* addr, size_t addr_len);
	NetPacket* DequeueWrite();
	void EnqueueIncoming(NetPacket *packet);
	void EnqueueIncoming(uint32_t* buffer, size_t len, sockaddr* addr, size_t addr_len);
	NetPacket* DequeueRead();
	void DestroyAll();
};
