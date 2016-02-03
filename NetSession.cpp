#include "NetSession.hpp"
#include "NetAddress.hpp"
#include "NetConnection.hpp"

void NetSession::Shutdown()
{
	NetSystem *system = NetSystem::GetInstance();

	for (unsigned int i = 0; i < m_sockets.size(); ++i) {
		UDPSocket *sock = m_sockets[i];
		system->FreeSocket(sock);
	}

	m_sockets.clear();
	m_packetQueue.DestroyAll();

	m_listening = false;
}

bool NetSession::Host(short port)
{
		NetSystem* system = NetSystem::GetInstance();

		UDPSocket *sock = system->CreateUDPSocket(&m_packetQueue, port);
		m_sockets.push_back(sock);

		return true;
}

void NetSession::Listen(bool listening)
{
	m_listening = listening;
}

void NetSession::SendPacket(NetPacket* packet) {
	m_packetQueue.EnqueueOutgoing(packet);
}

void NetSession::ReceivePacket(NetPacket* packet) {
	m_packetQueue.EnqueueIncoming(packet);
}

NetConnection* NetSession::AddConnection(const NetAddress& addr) {
	NetConnection* connection = new NetConnection();
	connection->m_netAddress = addr;
	m_connections.push_back(connection);
	return connection;
}

NetConnection* NetSession::FindConnection(const NetAddress& addr) {
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		NetAddress na = nc->m_netAddress;
		if (nc->m_netAddress == na) {
			return nc;
		}
	}
	return nullptr;
}