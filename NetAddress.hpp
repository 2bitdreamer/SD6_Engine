#pragma once
#include <winsock2.h>
#include "Utilities\EngineCommon.hpp"
struct sockaddr;

class NetAddress
{
public:
	uint32_t m_addr;;
	uint16_t m_port;

	void Init(sockaddr* saddr);


};

inline bool operator==(const NetAddress& lhs, const NetAddress& rhs) {
	return (lhs.m_port == rhs.m_port) && (lhs.m_addr == rhs.m_addr);
}