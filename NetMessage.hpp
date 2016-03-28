#pragma once
#include <stdint.h>
#include <string>
#include "NetPacket.hpp"
#include "NetConnection.hpp"
class NetPacket;
class NetConnection;
class NetMessage;

constexpr size_t const NetMessage_MTU = 1024;

typedef void(NetMessageReceived)(net_sender_t& from, NetMessage* msg);

//JoinRequestCallback

struct NetMessageDefinition {
	NetMessageDefinition() :
		m_id(0),
		m_options(0),
		m_callback(nullptr)
	{
	}
	
	NetMessageDefinition(NetMessageDefinition* msg) {
		m_callback = msg->m_callback;
		m_id = msg->m_id;
		m_name = msg->m_name;
		m_options = msg->m_options;
	}

	uint32_t m_options;
	std::string m_name;
	uint8_t m_id;
	bool IsConnectionless() const;
	bool IsReliable() const;
	NetMessageReceived* m_callback;
};

enum eNetMessageOptions
{
	eNMO_Reliable = (1 << 0),
	eNMO_Connectionless = (1 << 1),
	eNMO_InOrder = ( 1 << 2)
	// MORE WILL COME!  <scary music...>
};

class NetMessage : public ByteBuffer
{
public:
	NetMessage(uint8_t id);
	NetMessage(NetPacket& packet);
	NetMessage();

	NetMessage(NetMessage* msgToCopy);
	~NetMessage();

	static uint8_t GetNextID();
	static NetMessageDefinition* GetNetMessageDefinitionByID(uint8_t id);
	bool IsReliable() const;
	bool IsInOrder() const;
	size_t ComputeHeaderSize() const;
	static NetMessageDefinition* GetNetMessageDefinitionByName(const std::string& name);
	static void RegisterMessageDefinition(uint8_t id, NetMessageDefinition def);
	size_t GetRequiredSpaceInPacket() const;
	void SetMessageData(void* data, size_t dataLen);

public:
	NetMessageDefinition* m_messageDefinition;

	uint16_t m_inOrderID;
	uint16_t m_reliableID;
	double m_lastSentTime;
};


