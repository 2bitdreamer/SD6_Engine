#include "NetMessage.hpp"
#include "NetPacket.hpp"
#include <map>
#include "Utilities\DevConsole.hpp"
#include "NetSession.hpp"

static std::map<uint8_t, NetMessageDefinition> g_messageDefinitions;

NetMessage::NetMessage(uint8_t id) : 
	m_inOrderID(0),
	m_reliableID(0)
{
	m_messageDefinition = NetMessage::GetNetMessageDefinitionByID(id);
	FATAL_ASSERT(m_messageDefinition != nullptr);
	Init(m_buffer, NetMessage_MTU);
	//WriteBytes((void*)id, sizeof(uint8_t));
}

NetMessageDefinition* NetMessage::GetNetMessageDefinitionByID(uint8_t id)
{
	auto result = g_messageDefinitions.find(id);
	if (result != g_messageDefinitions.end())
		return &result->second;
	return nullptr;
}

bool NetMessage::IsReliable() const {
	return ((m_messageDefinition->m_options & eNMO_Reliable) != 0);
}

bool NetMessage::IsInOrder() const {
	return ((m_messageDefinition->m_options & eNMO_InOrder) != 0);
}


size_t NetMessage::ComputeHeaderSize() const
{
	size_t size = 2; //Message Size
	size += 1; // Message Type
	if ((m_messageDefinition->m_options & eNMO_Reliable) != 0) //If reliable, add 2
		size += 2;
	return size;
}

NetMessage::NetMessage(NetMessage* msgToCopy) {
	Init(m_buffer, NetMessage_MTU);
	memcpy(m_buffer, msgToCopy->GetBuffer(), msgToCopy->GetLength());
	m_messageDefinition = msgToCopy->m_messageDefinition;
	m_numBytesWritten = msgToCopy->m_numBytesWritten;
	m_maxSize = msgToCopy->m_maxSize;
	m_reliableID = msgToCopy->m_reliableID;
	m_inOrderID = msgToCopy->m_inOrderID;
}

NetMessage::NetMessage(NetPacket& packet)
{
	//Will read the length and id off the packet, and assert the id is valid.  Will also set max readable bytes.
	(void)packet;
}

NetMessage::NetMessage()
{
	Init(m_buffer, PACKET_MTU);
}

NetMessageDefinition* NetMessage::GetNetMessageDefinitionByName(const std::string& name) {
	for (auto it = g_messageDefinitions.begin(); it != g_messageDefinitions.end(); ++it) {
		NetMessageDefinition res = it->second;
		if (res.m_name == name)
			return &it->second;
	}

	return nullptr;
}



void NetMessage::RegisterMessageDefinition(uint8_t id, NetMessageDefinition def)
{
	def.m_id = id;
	g_messageDefinitions.insert(std::make_pair(id, def));
}

// HIGH LEVEL VIEW OF OUR GOAL
void NetJoin(NetAddress* addr)
{
// 	NetSession* sp = new NetSession();
// 	sp->Start();
// 
// 	sp->Join(addr);
// 
// 	// spin while we wait for session to join or fail
// 	while (!sp->is_connected() && !sp->is_disconnected()) {
// 	}
// 
// 	if (sp->is_connected()) {
// 		// Move on to game
// 	}
// 	else {
// 		// Move back to title screen with error - could not connect
// 		NetSessionDestroy(sp);
// 	}
}

size_t NetMessage::GetRequiredSpaceInPacket() const
{
	return m_numBytesWritten;
}

void NetMessage::SetMessageData(void* data, size_t dataLen) {
	memcpy(m_buffer, data, dataLen);
	m_numBytesWritten += dataLen;
}

NetMessage::~NetMessage()
{
}

uint8_t NetMessage::GetNextID()
{
	static uint8_t id = 0;
	return id++;
}

bool NetMessageDefinition::IsConnectionless() const
{
	return (m_options & eNMO_Connectionless) != 0;
}

bool NetMessageDefinition::IsReliable() const
{
	return (m_options & eNMO_Reliable) != 0;
}
