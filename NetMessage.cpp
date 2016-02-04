#include "NetMessage.hpp"
#include "NetPacket.hpp"
#include <map>

static std::map<uint8_t, NetMessageDefinition> g_messageDefinitions;

NetMessage::NetMessage(uint8_t id) {
	m_messageDefinition = NetMessage::GetNetMessageDefinitionByID(id);
	Init(&m_buffer, NetMessage_MTU);
}

NetMessage::NetMessage(NetPacket& packet) 
{

}

NetMessageDefinition* NetMessage::GetNetMessageDefinitionByID(uint8_t id) {
	auto result = g_messageDefinitions.find(id);
	if (result != g_messageDefinitions.end())
		return &result->second;
	return nullptr;
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
