#pragma once
#include <stdint.h>
class NetPacket;
class NetConnection;
class NetMessage;

typedef void(NetMessageReceived)(NetConnection*, NetMessage& msg);

class NetMessage
{
public:
	NetMessage(uint8_t id);
	NetMessage(NetPacket& packet);
	~NetMessage();

	static NetMessageReceived* GetNetMessageDefinitionByID(uint8_t id);
	void RegisterMessageDefinition(uint8_t id, NetMessageReceived* func);

public:
	uint8_t m_id;
	NetMessageReceived* m_callback;

	
};


