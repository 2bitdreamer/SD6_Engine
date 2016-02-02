

#include <string>

#include "UDPSocket.hpp"
#include "NetPacket.hpp"
#include "PacketQueue.hpp"
#include "SocketThread.hpp"

UDPSocket::UDPSocket(NetPacketQueue *queue, short port)
{
	m_running = true;

	m_service_thread = new SocketThread();
	m_service_thread->Init(this, queue, port);
	m_service_thread->Start("SocketThread");
}
 
bool UDPSocket::IsRunning() const
{
	return m_running;
}

void UDPSocket::Close()
{
	m_running = false;
}

void UDPSocket::Join()
{
		Close();
		m_service_thread->Join();
}