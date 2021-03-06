#include "PacketQueue.hpp"
#include "NetPacket.hpp"

void NetPacketQueue::EnqueueOutgoing(unsigned char *buffer, size_t len, sockaddr* addr)
{
	NetPacket *packet = new NetPacket(buffer, len, addr);
	packet->SetLength(len);
	EnqueueOutgoing(packet);
}

void NetPacketQueue::EnqueueOutgoing(NetPacket *packet)
{
	m_outgoing.enqueue(packet);
}

NetPacket* NetPacketQueue::DequeueWrite()
{
	NetPacket *packet = nullptr;
	if (m_outgoing.dequeue(&packet)) {
		return packet;
	}
	
	return nullptr;
}

void NetPacketQueue::EnqueueIncoming(unsigned char* buffer, size_t len, sockaddr* addr)
{
	NetPacket *packet = new NetPacket(buffer, len, addr);
	packet->SetLength(len);
	EnqueueIncoming(packet);
}

void NetPacketQueue::EnqueueIncoming(NetPacket *packet)
{
	m_incoming.enqueue(packet);
}

NetPacket* NetPacketQueue::DequeueRead()
{
	NetPacket* packet = nullptr;
	if (m_incoming.dequeue(&packet)) {
		return packet;
	}

	return nullptr;
}

void NetPacketQueue::DestroyAll()
{
	for (unsigned int num = 0; num < m_incoming.size(); num++) {
		NetPacket* pack = nullptr;
		m_incoming.dequeue(&pack);
		if (pack)
			delete pack;
	}

	for (unsigned int num = 0; num < m_incoming.size(); num++) {
		NetPacket* pack = nullptr;
		m_outgoing.dequeue(&pack);
		if (pack)
			delete pack;
	}
}
