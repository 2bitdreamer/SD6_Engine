#include "NetMessage.hpp"
#include "NetPacket.hpp"
#include <map>

static std::map<uint8_t, NetMessageReceived*> g_messageDefinitions;

NetMessage::NetMessage(uint8_t id)
{
}

NetMessage::NetMessage(NetPacket& packet) 
{

}

NetMessageReceived* NetMessage::GetNetMessageDefinitionByID(uint8_t id) {
	auto result = g_messageDefinitions.find(id);
	if (result != g_messageDefinitions.end())
		return result->second;
}

void NetMessage::RegisterMessageDefinition(uint8_t id, NetMessageReceived* func)
{
	g_messageDefinitions.insert(std::make_pair(id, func));
}

NetMessage::~NetMessage()
{
}
