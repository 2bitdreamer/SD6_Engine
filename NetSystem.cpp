#include "NetSystem.hpp"
#include <corecrt_malloc.h>
#include "UDPSocket.hpp"
#include "NetSession.hpp"
#include "NetMessage.hpp"
#include "Utilities\DevConsole.hpp"
#include "NetConnection.hpp"

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
		NetMessageDefinition pingDef;
		pingDef.m_callback = PingCallback;
		pingDef.m_name = "ping";
		NetMessage::RegisterMessageDefinition(0, pingDef);

		NetMessageDefinition pongDef;
		pongDef.m_callback = PongCallback;
		pongDef.m_name = "pong";
		NetMessage::RegisterMessageDefinition(1, pongDef);

		return true;
	}
	else {
		FATAL_ERROR("Could not setup WSA System");
		return false;
	}

	//Create netmessage definitions here
}

void NetSystem::Deinit()
{
	WSACleanup();
}

NetSystem::NetSystem()
{
	Init();

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
	m_networkSessions.push_back(s);
	return s;
}

void NetSystem::DestroySession(NetSession* s)
{
	free(s);
}

void PingCallback(NetConnection* nc, NetMessage* msg) {
	DevConsole* devC = DevConsole::GetInstance();
	devC->ConsolePrintf("%s %s", RGBA(255, 0, 0, 255), "Received ping from ", nc->m_netAddress);
}

void PongCallback(NetConnection* nc, NetMessage* msg) {
	DevConsole* devC = DevConsole::GetInstance();
	devC->ConsolePrintf("%s %s", RGBA(0, 255, 0, 255), "Received pong from ", nc->m_netAddress);
}

void NetSystem::Tick()
{
	for (auto it = m_networkSessions.begin(); it != m_networkSessions.end(); ++it) {
		NetSession* ns = *it;
		ns->Tick();
	}
}