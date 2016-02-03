#pragma once
#include <stdint.h>
#include <string>
#include "NetPacket.hpp"
class NetPacket;
class NetConnection;
class NetMessage;

constexpr size_t const NetMessage_MTU = 1024;

typedef void(NetMessageReceived)(NetConnection*, NetMessage& msg);

struct NetMessageDefinition {
	std::string m_name;
	uint8_t m_id;
	NetMessageReceived* m_callback;
};

class NetMessage : public ByteBuffer
{
public:
	NetMessage(uint8_t id);
	NetMessage(NetPacket& packet);
	~NetMessage();

	static uint8_t GetNextID();
	static NetMessageDefinition* GetNetMessageDefinitionByID(uint8_t id);
	static NetMessageDefinition* GetNetMessageDefinitionByName(const std::string& name);
	static void RegisterMessageDefinition(uint8_t id, NetMessageDefinition def);
	size_t GetRequiredSpaceInPacket() const;
public:
	unsigned char m_buffer[NetMessage_MTU];
	NetMessageDefinition* m_messageDefinition;
};


