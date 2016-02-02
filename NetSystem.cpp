#include "NetSystem.hpp"
#include <corecrt_malloc.h>
#include "UDPSocket.hpp"
#include "NetSession.hpp"

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

