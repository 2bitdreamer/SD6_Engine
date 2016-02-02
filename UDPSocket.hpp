#pragma once
#include "Utilities\EngineCommon.hpp"
#include "Thread.hpp"

class SocketThread;
class NetPacketQueue;

// HEADER FILE
class UDPSocket
{
private:
	bool m_running;
	SocketThread* m_service_thread;

protected:

public:
	UDPSocket(NetPacketQueue *queue, short port);

	bool IsRunning() const;
	void Close();

	// Blocks on this socket until it's thead is finished
	void Join();

};

