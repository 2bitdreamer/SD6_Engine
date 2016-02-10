#include "NetAddress.hpp"
#include <windows.h>

#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif

#include <winsock2.h>
#include "Assert.hpp"
#include <ws2tcpip.h>

void NetAddress::Init(sockaddr* saddr)
{
	sockaddr_in* ain = (sockaddr_in*)saddr;
	m_port = ain->sin_port;
	inet_ntop(AF_INET, &ain->sin_addr, (PSTR)m_addr, 16);
}

