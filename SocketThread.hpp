#include "Thread.hpp"

#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif

#include <winsock2.h>
// Declared in source file
// Engine is only one that sees the socket thread
class UDPSocket;
class NetPacketQueue;

class SocketThread : public Thread
{
private:
	SOCKET m_thisSocket;
	short m_port;
	UDPSocket* m_owner;

	// owned by a session, which is uses to read and write
	NetPacketQueue *m_packetQueue;

	void CreateAndBindSocket();
	void ProcessIncoming();
	void ProcessOutgoing();
	void ServiceSocket();

protected:
	virtual void Run();

public:
	void Init(UDPSocket* owner, // socket that owns this service thread
		NetPacketQueue* queue,    // queue it will read and write from
		short port                 // port to use.
		);
};

