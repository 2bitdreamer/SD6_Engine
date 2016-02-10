#pragma once
#include <stdint.h>
#include <vector>
#include "NetAddress.hpp"
#include "Utilities\ThreadSafeQueue.hpp"
class NetMessage;
class NetSession;

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void SendMessage(NetMessage *msg);
	void Tick();
public:
	uint16_t m_nextAckID;
	ThreadSafeQueue<NetMessage*> m_outgoingMessages;
	float m_lastSentTime;
	float m_tickFrequency;
	NetAddress m_netAddress;
	NetSession* m_owningSession;
};

