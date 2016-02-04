#include "NetSystem.hpp"
#include <corecrt_malloc.h>
#include "UDPSocket.hpp"
#include "NetSession.hpp"
#include "NetMessage.hpp"

NetSystem* NetSystem::GetInstance()
{
	static NetSystem* ns = new NetSystem();
	return ns;
}

bool NetSystem::Init()
{
	WSAData wsa_data;
	int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (error == 0) {
		return true;
	}
	else {
		FATAL_ERROR("Could not setup WSA System");
		return false;
	}
}

NetSystem::NetSystem()
{

	Init();
	uint8_t nextMessageID = NetMessage::GetNextID();
	NetMessage nm(nextMessageID);
	NetMessageDefinition pingDef;
	pingDef.m_callback = Ping;
	pingDef.m_name = "ping";
	NetMessage::RegisterMessageDefinition(nextMessageID, pingDef);
}

UDPSocket* NetSystem::CreateUDPSocket(NetPacketQueue *queue, short port)
{
	UDPSocket *sock = new UDPSocket(queue, port);
	return sock;
}

void NetSystem::FreeSocket(UDPSocket* sock)
{
	sock->Join();
	delete sock;
}

NetSession* NetSystem::CreateSession()
{
	NetSession *s = new NetSession();
	return s;
}

void NetSystem::DestroySession(NetSession* s)
{
	free(s);
}

void Ping(NetConnection* nc, NetMessage& msg)
{
	std::cout << "Ping Called!!!" << std::endl;
}
