#pragma once
#include <stdint.h>
class NetPacket;
class NetConnection;
class NetMessage;

typedef void(NetMessageReceived)(NetConnection*, NetMessage& msg);

struct NetMessageDefinition {
	std::string m_name;
	uint8_t m_id;
	NetMessageReceived* m_callback;
};

class NetMessage
{
public:
	NetMessage(uint8_t id);
	NetMessage(NetPacket& packet);
	~NetMessage();

	static NetMessageDefinition* GetNetMessageDefinitionByID(uint8_t id);
	static void RegisterMessageDefinition(uint8_t id, NetMessageDefinition def);
	
};


