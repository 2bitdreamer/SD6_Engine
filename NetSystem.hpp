#pragma once

class NetSession;
class UDPSocket;
class NetPacketQueue;
class NetConnection;
class NetMessage;

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
	NetSession* CreateSession();
	void DestroySession(NetSession* s);
};

void Ping(NetConnection* nc, NetMessage& msg);




