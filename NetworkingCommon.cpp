#include "NetworkingCommon.hpp"
#include "NetAddress.hpp"
#include <ws2tcpip.h>
#include "Utilities\DevConsole.hpp"


size_t NetAddressForHost(NetAddress* outbuf, size_t outbuf_len, int family, const std::string& hostname, uint16_t port, bool bindable)
{
	(void)family;
	addrinfo* addresses = AllocAddressesForHost(hostname.c_str(), std::to_string(port).c_str(), AF_INET, SOCK_DGRAM, bindable);

	size_t c = 0;
	addrinfo* addr = addresses;

	while ((c < outbuf_len) && (addr != nullptr)) {

		if (addr->ai_family == AF_INET) {
			NetAddress na;
			NetAddrFromSockAddr(&na, addr->ai_addr);
			outbuf[c] = na;
			++c;
		}

		addr = addr->ai_next;
	}

	FreeAddresses(addresses);
	return c;
}

// get sockaddr, IPv4 or IPv6:
void* GetInAddr(sockaddr const *sa)
{
	return &(((sockaddr_in*)sa)->sin_addr);
}

void* GetInAddr(sockaddr *sa, uint16_t *port, size_t *size_out)
{
	*size_out = sizeof(((sockaddr_in*)sa)->sin_addr);
	*port = ((sockaddr_in*)sa)->sin_port;
	return &(((sockaddr_in*)sa)->sin_addr);
}

void NetAddrFromSockAddr(NetAddress *na, sockaddr *addr)
{
	memset(na, 0, sizeof(NetAddress));
	size_t addrlen;
	void *addr_ptr = GetInAddr(addr, &(na->m_port), &addrlen);
	FATAL_ASSERT(addrlen <= sizeof(na->m_addr));
	memcpy(&na->m_addr, addr_ptr, addrlen);
}


//------------------------------------------------------------------------
void SockAddrFromNetAddr(sockaddr *addr, size_t *addrlen, NetAddress const &net_addr)
{
	sockaddr_in* sa = (sockaddr_in*)addr;
	memset(sa, 0, sizeof(sockaddr_in));
	sa->sin_family = AF_INET;
	inet_pton(AF_INET, (PCSTR)net_addr.m_addr, &sa->sin_addr);
	*addrlen = sizeof(sockaddr_in);
	sa->sin_port = htons(net_addr.m_port);
}

addrinfo* AllocAddressesForHost(char const *host,
	char const *service,
	int family,
	int socktype,
	bool binding)
{
	addrinfo hints;
	addrinfo *addr;

	if (nullptr == host) {
		host = "localhost";
	}

	// Which network layer it's using - usually want to UNSPEC, since it doesn't matter.  But since we're hard coding
	// the client sides connection, we will likely want to use AF_INET when we want to bind an address

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = family;

	hints.ai_socktype = socktype; // STREAM based, determines transport layer (TCP)
	hints.ai_flags = binding ? AI_PASSIVE : 0; // used for binding/listening

	int status = getaddrinfo(host, service, &hints, &addr);
	if (status != 0) {
		return nullptr;
	}

	return addr;
}

//-------------------------------------------------------------------------------------------------------
void FreeAddresses(addrinfo *addresses)
{
	freeaddrinfo(addresses);
}

//-------------------------------------------------------------------------------------------------------
uint16_t GetAddressPort(sockaddr const *sa)
{
	USHORT port = 0;
	port = (((sockaddr_in*)sa)->sin_port);
	return ntohs(port);
}

//-------------------------------------------------------------------------------------------------------
size_t GetAddressName(char *buffer, size_t const buffer_size, sockaddr const *sa)
{
	char addr_name[INET6_ADDRSTRLEN];
	memset(addr_name, 0, sizeof(addr_name));
	inet_ntop(sa->sa_family, GetInAddr(sa), addr_name, INET6_ADDRSTRLEN);

	uint16_t port = GetAddressPort(sa);

	//size_t len = min(buffer_size - 1, strlen(addr_name));
	return sprintf_s(buffer, buffer_size, "%s:%i", addr_name, port);
}

//-------------------------------------------------------------------------------------------------------
void ForEachAddress(addrinfo *addresses, address_work_cb cb, void *user_arg)
{
	addrinfo *iter = addresses;
	while (nullptr != iter) {
		if (cb(iter, user_arg)) {
			break;
		}

		iter = iter->ai_next;
	}
}