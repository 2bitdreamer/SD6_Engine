#include "SocketThread.hpp"
#include "NetPacket.hpp"
#include "PacketQueue.hpp"
#include "UDPSocket.hpp"

void SocketThread::CreateAndBindSocket()
{
	m_thisSocket = INVALID_SOCKET;

	// create the socket
	SOCKET sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	FATAL_ASSERT(sock != INVALID_SOCKET); // make better error handling

										  // 2. Bind the socket to whatever address is available, on the port supplied
										  // 
										  // Setup the address to use
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(m_port);
	addr.sin_family = PF_INET;

	int status = bind(sock, (sockaddr*)&addr, sizeof(addr));
	if (status == SOCKET_ERROR) {
		FATAL_ERROR("Failed to bind address");
		closesocket(sock);
		return;
	}

	m_thisSocket = sock;

	// Set to non_blocking
	unsigned long non_blocking = 1;
	ioctlsocket(m_thisSocket, FIONBIO, &non_blocking);
}

void SocketThread::ProcessIncoming()
{
	unsigned char buffer[PACKET_MTU];
	sockaddr_storage their_addr;
	while (m_owner->IsRunning()) {
		size_t addrlen = sizeof(their_addr);
		int recvd = recvfrom(m_thisSocket,   // socket we're reading on
			(char*)buffer,                    // buffer to read into 
			PACKET_MTU,                // max size of buffer
			0,                         // options
			(sockaddr*)&their_addr,    // Address we received from on success
			(int*)&addrlen);                // Address size in/out (storage/result)

		if (recvd > 0) {
			sockaddr_in* addr = (sockaddr_in*)&their_addr;
			addr->sin_port = ntohs(addr->sin_port);
			m_packetQueue->EnqueueIncoming(buffer, recvd, (sockaddr*)addr);
		}
		else if (recvd < 0) {
			int error = WSAGetLastError();
			switch (error) {
			case WSAEWOULDBLOCK: // nothing to read
			case WSAEMSGSIZE: // was too large, truncated, throw it out
				break;
			default:
				m_owner->Close();
			};
			return;
		}
		else {
			m_owner->Close();
		}
	}
}



void SocketThread::ProcessOutgoing()
{
	NetPacket *packet = m_packetQueue->DequeueWrite();
	while (nullptr != packet) {

		sockaddr_storage sockstorage;
		size_t addrlen;
		SockAddrFromNetAddr((sockaddr*)(&sockstorage), &addrlen, packet->m_address);

		int sent = sendto(m_thisSocket,
			(char*)packet->GetBuffer(),
			packet->GetLength(),
			0,
			(sockaddr*)&sockstorage,
			addrlen);

		if (sent == SOCKET_ERROR)
			int err = WSAGetLastError();
		
		FATAL_ASSERT(sent != SOCKET_ERROR);

		delete packet;
		packet = m_packetQueue->DequeueWrite();
	}
}

void SocketThread::ServiceSocket()
{
	while (m_owner->IsRunning()) {
		// Try to recv data, read until nothing there
		// or it errored out
		ProcessIncoming();
		ProcessOutgoing();

		ThreadYield();
	}
}

void SocketThread::Run()
{
	CreateAndBindSocket();
	if (m_thisSocket != INVALID_SOCKET) {
		ServiceSocket();
		closesocket(m_thisSocket);
	}
}

void SocketThread::Init(UDPSocket *owner, /* socket that owns this service thread */ NetPacketQueue *queue, /* queue it will read and write from */ short port /* port to use. */)
{
	this->m_owner = owner;
	this->m_thisSocket = INVALID_SOCKET;
	this->m_port = port;
	this->m_packetQueue = queue;
}
