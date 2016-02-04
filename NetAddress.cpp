#include "NetAddress.hpp"
#include <windows.h>
#include <winsock.h>
#include "Assert.hpp"

void NetAddress::Init(sockaddr* saddr)
{
// 	sockaddr_in* ain = (sockaddr_in*)saddr;
// 	memcpy((void*)m_addr, ain->sin_addr.S_un.S_addr, m_)
// 	m_port = ain->sin_port;
// 	FATAL_ASSERT(ain->sin_family == AF_INET);
}

