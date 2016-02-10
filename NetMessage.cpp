#include "NetMessage.hpp"
#include "NetPacket.hpp"
#include <map>
#include "Utilities\DevConsole.hpp"

static std::map<uint8_t, NetMessageDefinition> g_messageDefinitions;

NetMessage::NetMessage(uint8_t id) {
	m_messageDefinition = NetMessage::GetNetMessageDefinitionByID(id);
	FATAL_ASSERT(m_messageDefinition != nullptr);
	Init(&m_buffer, NetMessage_MTU);
	//WriteBytes((void*)id, sizeof(uint8_t));
}

NetMessageDefinition* NetMessage::GetNetMessageDefinitionByID(uint8_t id) {
	auto result = g_messageDefinitions.find(id);
	if (result != g_messageDefinitions.end())
		return &result->second;
	return nullptr;
}

NetMessage::NetMessage(NetMessage* msgToCopy) {
	Init(&m_buffer, NetMessage_MTU);
	memcpy(&m_buffer, msgToCopy->GetBuffer(), msgToCopy->GetLength());
	m_messageDefinition = msgToCopy->m_messageDefinition;
	m_numBytesWritten = msgToCopy->m_numBytesWritten;
	m_maxSize = msgToCopy->m_maxSize;
}

NetMessage::NetMessage(NetPacket& packet)
{
	//Will read the length and id off the packet, and assert the id is valid.  Will also set max readable bytes.
	(void)packet;
}

NetMessage::NetMessage()
{
	Init(&m_buffer, PACKET_MTU);
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