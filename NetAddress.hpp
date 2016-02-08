#pragma once

#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif

#include <winsock2.h>
#include "Utilities\EngineCommon.hpp"
struct sockaddr;

class NetAddress
{
public:
	unsigned char m_addr[16];
	unsigned short m_port;

	void Init(sockaddr* saddr);

	NetAddress() :
		m_port(0),
		m_addr()
	{

	}
};

inline bool operator==(const NetAddress& lhs, const NetAddress& rhs) {
	return (lhs.m_port == rhs.m_port) && (lhs.m_addr == rhs.m_addr);
}