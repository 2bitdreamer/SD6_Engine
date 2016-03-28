#pragma once
#include "NetSystem.hpp"
#include <vector>
#include "PacketQueue.hpp"
#include "NetConnection.hpp"
class UDPSocket;
class NetAddress;

// ENGINE
class NetSession
{
public:
	std::vector<UDPSocket*> m_sockets;
	std::vector<NetConnection*> m_connections;
	NetConnection* m_me;
	NetConnection* m_hostConnection;
	NetPacketQueue m_packetQueue;
	int m_maxConnections;
	bool m_listening; // are we accepting new connections

public:

	NetSession();
	void Shutdown();

	// Starts the session as a host
	bool Host(short port);

	// Sets the session as accepting new connections or not
	void Listen(bool listening);

	void SendPacket(NetPacket* packet);
	void ReceivePacket(NetPacket* packet);
	void SendMsg(NetMessage* msg, NetConnection* nc);
	void Tick();
	NetConnection* AddConnection(const NetAddress& addr, bool isHost);
	NetConnection* FindConnectionByNetAddress(NetAddress* addr);
	NetConnection* FindConnectionByIndex(int connID);
	const char* GetHostAddrName() const;
	bool ValidatePacket(NetPacket* pack);
	void ExtractMessages(NetPacket* pack);
	void ForceTestAll();
	bool CanProcessMessage(uint16_t ack, net_sender_t& sender, NetMessage* msg);
	void RemoveConnection(NetConnection * connection);
	NetConnection* FindConnectionByID(const std::string& id);
};