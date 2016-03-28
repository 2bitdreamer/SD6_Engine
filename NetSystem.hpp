#pragma once
#include <vector>

class NetSession;
class UDPSocket;
class NetPacketQueue;
class NetConnection;
class NetMessage;
struct net_sender_t;

class NetSystem
{

public:
	static NetSystem* GetInstance();

	bool Init(); // WSAStartup
	void Deinit(); // WSACleanup
	NetSystem();

	UDPSocket* CreateUDPSocket(NetPacketQueue *queue, short port);
	// Be sure to call .join on the socket before freeing the memory
	// otherwise you will have thread issues.   
	void FreeSocket(UDPSocket* socket);
	NetSession* CreateSession(short port);
	void DestroySession(NetSession* s);
	void Tick();

public:
	std::vector<NetSession*> m_networkSessions;

};

void PingCallback(net_sender_t &from, NetMessage* msg);
void PongCallback(net_sender_t &from, NetMessage* msg);
void HeartbeatCallback(net_sender_t& from, NetMessage* msg);
void InOrderTestCallback(net_sender_t& from, NetMessage* msg);
void JoinAcceptCallback(net_sender_t& from, NetMessage* msg);
void JoinRequestCallback(net_sender_t& from, NetMessage* msg);
void StartTestCallback(net_sender_t& from, NetMessage* msg);
void JoinDenyCallback(net_sender_t& from, NetMessage* msg);
void AckCallback(net_sender_t& from, NetMessage* msg);
void LeaveCallback(net_sender_t& from, NetMessage* msg);
void ForceTestCallback(net_sender_t& from, NetMessage* msg);