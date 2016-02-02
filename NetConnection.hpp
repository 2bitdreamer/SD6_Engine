#pragma once
#include <stdint.h>
#include <vector>
#include "NetAddress.hpp"
class NetMessage;

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

public:
	uint16_t m_nextAckID;
	std::vector<NetMessage*> m_outgoingMessages;
	float m_lastSentTime;
	NetAddress m_netAddress;
};

