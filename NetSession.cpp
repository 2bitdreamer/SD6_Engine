#include "NetSession.hpp"
#include "NetAddress.hpp"
#include "NetConnection.hpp"
#include "NetMessage.hpp"
#include "Utilities\DevConsole.hpp"
#include "Utilities\Time.hpp"

void NetSession::Shutdown()
{
	NetSystem *system = NetSystem::GetInstance();

	for (unsigned int i = 0; i < m_sockets.size(); ++i) {
		UDPSocket *sock = m_sockets[i];
		system->FreeSocket(sock);
	}

	m_sockets.clear();
	m_packetQueue.DestroyAll();

	m_listening = false;
}

bool NetSession::Host(short port)
{
		NetSystem* system = NetSystem::GetInstance();

		UDPSocket *sock = system->CreateUDPSocket(&m_packetQueue, port);
		m_sockets.push_back(sock);

		return true;
}

void NetSession::Listen(bool listening)
{
	m_listening = listening;
}

void NetSession::SendPacket(NetPacket* packet) {
	m_packetQueue.EnqueueOutgoing(packet);
}

void NetSession::ReceivePacket(NetPacket* packet) {
	m_packetQueue.EnqueueIncoming(packet);
}

void NetSession::SendMessage(NetMessage* msg) {
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		NetMessage* nmCopy = new NetMessage(msg);
		nc->m_outgoingMessages.enqueue(nmCopy);
	}

	delete msg;
}

bool NetSession::ValidatePacket(NetPacket* pack)
{
	size_t packetLen = pack->GetLength();

	if (packetLen < 3) {
		DevConsole* devConsole = DevConsole::GetInstance();
		devConsole->ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Packet length does not include header");
		return false;
	}

	int byteAt = 0;
	//uint16_t ack = *(uint16_t*)pack->m_buffer;
	byteAt += sizeof(uint16_t);

	uint8_t messageCount = *(uint8_t*)(pack->m_buffer + byteAt);
	byteAt += sizeof(uint8_t);

	size_t totalMessagesLength = 0;

	for (uint8_t messageIndex = 0; messageIndex < messageCount; messageIndex++) {
		uint16_t messageLen = *(uint16_t*)(pack->m_buffer + byteAt);
		totalMessagesLength += messageLen;
		byteAt += messageLen;
	}

	if ((totalMessagesLength + 3) != (packetLen)) {
		DevConsole* devConsole = DevConsole::GetInstance();
		devConsole->ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Message data size not equal to size claimed");
		return false;
	}

	return true;

}

void NetSession::ExtractMessages(NetPacket* pack) {
	size_t byteAt = 2;
	uint8_t messageCount = *(uint8_t*)(pack->m_buffer + byteAt);
	byteAt += sizeof(uint8_t);

	for (uint8_t messageIndex = 0; messageIndex < messageCount; messageIndex++) {
		NetMessage* nm = new NetMessage();

		uint16_t messageLen = *(uint16_t*)(pack->m_buffer + byteAt);
		byteAt += sizeof(uint16_t);
		nm->SetLength(messageLen);

		uint8_t messageType = *(uint8_t*)(pack->m_buffer + byteAt);
		byteAt += sizeof(uint8_t);
		
		memcpy( nm->m_buffer, pack->m_buffer + byteAt, messageLen - 3);
		byteAt += (messageLen - 3);

		nm->m_messageDefinition = NetMessage::GetNetMessageDefinitionByID(messageType);
		nm->m_messageDefinition->m_callback(FindConnection(pack->GetAddress()), nm);
	}

}

void NetSession::Tick() {

	//Ticks all connections if time
	/*if (m_listening) {*/
		NetPacket* pack = m_packetQueue.DequeueRead();
		if (pack) {
			bool packetValid = ValidatePacket(pack);
			if (!packetValid) {
				DevConsole* devConsole = DevConsole::GetInstance();
				devConsole->ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Packet did not validate");
			}
			else {
				ExtractMessages(pack);
			}
		}

		for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
			NetConnection* conn = *it;
			if ((conn->m_lastSentTime + conn->m_tickFrequency) < GetAbsoluteTimeSeconds()) {
				conn->Tick();
			}
		}
}

NetConnection* NetSession::AddConnection(const NetAddress& addr) {
	NetConnection* connection = new NetConnection();
	connection->m_owningSession = this;
	connection->m_netAddress = addr;
	m_connections.push_back(connection);

	DevConsole* devConsole = DevConsole::GetInstance();
	devConsole->ConsolePrintf("%s %s %s %i", RGBA(0, 255, 0, 255), "Added connection to address ", addr.m_addr, "and port ", addr.m_port);

	return connection;
}

NetConnection* NetSession::FindConnection(NetAddress* addr) {
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		NetAddress* na = &nc->m_netAddress;
		if (addr == na) {
			return nc;
		}
	}

	return nullptr;
}