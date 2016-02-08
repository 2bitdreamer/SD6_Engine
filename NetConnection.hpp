#pragma once
#include <stdint.h>
#include <vector>
#include "NetAddress.hpp"
#include "Utilities\ThreadSafeQueue.hpp"
class NetMessage;

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void SendMessage(NetMessage *msg);

public:
	uint16_t m_nextAckID;
	ThreadSafeQueue<NetMessage*> m_outgoingMessages;
	float m_lastSentTime;
	NetAddress m_netAddress;
};

