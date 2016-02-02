#pragma once
#include "NetSystem.hpp"
#include <vector>
#include "PacketQueue.hpp"
class UDPSocket;
class NetAddress;
class NetConnection;

// ENGINE
class NetSession
{
private:
	NetPacketQueue m_packetQueue;
	std::vector<UDPSocket*> m_sockets;
	std::vector<NetConnection*> m_connections;

	bool m_listening; // are we accepting new connections

public:
	NetSession()
	{
		m_listening = false;
	}

	void Shutdown();

	// Starts the session as a host
	bool Host(short port);

	// Sets the session as accepting new connections or not
	void Listen(bool listening);

	void SendPacket(NetPacket* packet);
	void ReceivePacket(NetPacket* packet);
	NetConnection* AddConnection(const NetAddress& addr);
	NetConnection* FindConnection(const NetAddress& addr);
	const char* GetHostAddrName() const;
};