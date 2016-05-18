#pragma once

#ifdef _WINSOCKAPI_
#undef _WINSOCKAPI_
#endif

#include <winsock2.h>
#include <stdint.h>
#include <string>

struct addrinfo;

class NetAddress;

// TYPES ////////////////////////////////////////////////////////////////////
typedef bool(*address_work_cb)(addrinfo*, void *user_arg);

void NetAddrFromSockAddr(NetAddress *na, sockaddr *addr);
void SockAddrFromNetAddr(sockaddr *addr, size_t *addrlen, NetAddress const &net_addr);

size_t NetAddressForHost(NetAddress* outbuf, size_t outbuf_len, int family, const std::string& hostname, uint16_t port, bool bindable);
void* GetInAddr(sockaddr const *sa);

void* GetInAddr(sockaddr *sa, uint16_t *port, size_t *size_out);

// FUNCTION PROTOTYPES //////////////////////////////////////////////////////
addrinfo* AllocAddressesForHost(char const *host,
	char const *service,
	int family,
	int socktype,
	bool binding);

void FreeAddresses(addrinfo* addresses);

uint16_t GetAddressPort(sockaddr const *addr);
size_t GetAddressName(char *buffer, size_t const buffer_size, sockaddr const *sa);

void ForEachAddress(addrinfo *addresses, address_work_cb cb, void *user_arg);