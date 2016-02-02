#pragma once

class NetSession;
class UDPSocket;
class NetPacketQueue;

class NetSystem
{

public:
	static NetSystem* GetInstance()
	{
		static NetSystem* ns = new NetSystem();
		return ns;
	}


	bool Init(); // WSAStartup
	void Deinit(); // WSACleanup

	UDPSocket* CreateUDPSocket(NetPacketQueue *queue, short port);
	// Be sure to call .join on the socket before freeing the memory
	// otherwise you will have thread issues.   
	void FreeSocket(UDPSocket* socket);
	NetSession* CreateSession();
	void DestroySession(NetSession* s);
};




