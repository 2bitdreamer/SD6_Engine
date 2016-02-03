#include "NetMessage.hpp"
#include "NetPacket.hpp"
#include <map>

static std::map<uint8_t, NetMessageDefinition> g_messageDefinitions;

NetMessage::NetMessage(uint8_t id)
{
}

NetMessage::NetMessage(NetPacket& packet) 
{

}

NetMessageDefinition* NetMessage::GetNetMessageDefinitionByID(uint8_t id) {
	auto result = g_messageDefinitions.find(id);
	if (result != g_messageDefinitions.end())
		return &result->second;
}

void NetMessage::RegisterMessageDefinition(uint8_t id, NetMessageDefinition def)
{
	g_messageDefinitions.insert(std::make_pair(id, def));
}

NetMessage::~NetMessage()
{
}
