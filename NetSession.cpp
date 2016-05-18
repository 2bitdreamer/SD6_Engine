#include "NetSession.hpp"
#include "NetAddress.hpp"
#include "NetConnection.hpp"
#include "NetMessage.hpp"
#include "Utilities\DevConsole.hpp"
#include "Utilities\Time.hpp"

NetSession::NetSession() :
	m_listening(false),
	m_maxConnections(0),
	m_me(nullptr),
	m_hostConnection(nullptr)
{
	

}

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
		return true;
		(void)port;
}

void NetSession::Listen(bool listening)
{
	m_listening = listening;

	if (listening)
		m_maxConnections = 8;
	else
		m_maxConnections = 0;
}

void NetSession::SendPacket(NetPacket* packet) {
	m_packetQueue.EnqueueOutgoing(packet);
}

void NetSession::ReceivePacket(NetPacket* packet) {
	m_packetQueue.EnqueueIncoming(packet);
}

void NetSession::SendMsg(NetMessage* msg, NetConnection* nc) {
	nc->SendMsg(msg);
}

bool NetSession::ValidatePacket(NetPacket* pack)
{
	size_t packetLen = pack->GetLength();

	if (packetLen < 3) {
		DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Packet length does not include header");
		return false;
	}

	int byteAt = 0;
	//uint16_t ack = *(uint16_t*)pack->m_buffer;

	byteAt += sizeof(uint8_t);

	byteAt += sizeof(uint16_t);

	uint8_t messageCount = *(uint8_t*)(pack->m_buffer + byteAt);
	byteAt += sizeof(uint8_t);

	size_t totalMessagesLength = 0;

	for (uint8_t messageIndex = 0; messageIndex < messageCount; messageIndex++) {
		uint16_t messageLen = *(uint16_t*)(pack->m_buffer + byteAt);
		totalMessagesLength += messageLen;
		byteAt += messageLen;
	}

	if ((totalMessagesLength + 4) != (packetLen)) {
		DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Message data size not equal to size claimed");
		return false;
	}

	return true;

}

void NetSession::ExtractMessages(NetPacket* pack) {

	size_t byteAt = 0;

	uint8_t connIdex = *(uint8_t*)(pack->m_buffer + byteAt);
	byteAt += sizeof(uint8_t);

	if (m_me == m_hostConnection && connIdex == 0) //No i will not connect to myself. DIAF.
		return;

	uint16_t ack = *(uint16_t*)(pack->m_buffer + byteAt);
	byteAt += sizeof(uint16_t);

	uint8_t messageCount = *(uint8_t*)(pack->m_buffer + byteAt);
	byteAt += sizeof(uint8_t);
	                      
	net_sender_t sender;

	NetConnection* nc = FindConnectionByIndex(connIdex);

	sender.session = this;

	if (nc) {
		nc->m_timeLastReceivedPacket = GetAbsoluteTimeSeconds();
		sender.address = nc->m_netAddress;
	}
	else {
		sender.address = pack->m_address;
	}
	
	sender.connection = nc;

	bool shouldAck = false;

	for (uint8_t messageIndex = 0; messageIndex < messageCount; messageIndex++) {
		NetMessage* nm = new NetMessage();

		uint16_t messageLen = *(uint16_t*)(pack->m_buffer + byteAt);
		byteAt += sizeof(uint16_t);
		nm->SetLength(messageLen);

		uint8_t messageType = *(uint8_t*)(pack->m_buffer + byteAt);
		byteAt += sizeof(uint8_t);

		NetMessageDefinition* msgDef = NetMessage::GetNetMessageDefinitionByID(messageType);
		bool isReliable = ((msgDef->m_options & eNMO_Reliable) != 0);
		nm->m_messageDefinition = msgDef;

		if (isReliable)
		{
			uint16_t reliableID = *(uint16_t*)(pack->m_buffer + byteAt);
			nm->m_reliableID = reliableID;
			byteAt += sizeof(uint16_t);
		}
		
		size_t headerSize = nm->ComputeHeaderSize();
		memcpy( nm->m_buffer, pack->m_buffer + byteAt, messageLen - headerSize);
		byteAt += (messageLen - headerSize);

		shouldAck = shouldAck || isReliable;
		if (!CanProcessMessage(ack, sender, nm))
			continue;

		nm->m_messageDefinition->m_callback(sender, nm);

		if (sender.connection)
			sender.connection->m_timeLastReceivedPacket = GetAbsoluteTimeSeconds();

		if (shouldAck && sender.connection != nullptr) {
			sender.connection->TrackAck(ack);
		}
	}

}


void NetSession::ForceTestAll() {
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		if (!nc->IsMe())
			nc->StartForceTest(8);
	}
}

bool NetSession::CanProcessMessage(uint16_t ack, net_sender_t& sender, NetMessage* msg)
{
	// Message Option:  CONNECTIONLESS
	bool isConnectionBound = !msg->m_messageDefinition->IsConnectionless();
	bool invalidConnection = sender.connection == nullptr;

	if (isConnectionBound && invalidConnection)
		return false;

	// Message Option: RELIABLE
	if (msg->m_messageDefinition->IsReliable() && (ack != 0xff)) {
		NetConnection *cp = sender.connection;
		if (cp != nullptr) {
			if (cp->m_receivedReliableIDs.get(msg->m_reliableID) == true) {
				return false;
			}
		}
	}

	return true;
}

void NetSession::RemoveConnection(NetConnection * connection)
{
	for (auto it = m_connections.begin(); it != m_connections.end(); ) {
		NetConnection* conn = *it;
		if (conn == connection) {
			it = m_connections.erase(it);
			conn->SetState(eConnectionState_Disconnected);
			delete conn;
		}
		else
			++it;
	}
}

NetConnection* NetSession::FindConnectionByID(const std::string& idc)
{
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		std::string id = nc->m_connectionID;
		if (idc == id)
			return nc;
	}

	return nullptr;
}

void NetSession::Tick() {

	//Ticks all connections if time
	/*if (m_listening) {*/
		NetPacket* pack = m_packetQueue.DequeueRead();
		if (pack) {
			NetConnection* connection = FindConnectionByNetAddress(&pack->m_address);
			if (connection)
				connection->m_timeLastReceivedPacket = GetAbsoluteTimeSeconds();

			bool packetValid = ValidatePacket(pack);
			if (!packetValid) {
				DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Packet did not validate");
				delete pack;
			}
			else {
				ExtractMessages(pack);
			}
		}

		for (auto it = m_connections.begin(); it != m_connections.end(); ) {
			NetConnection* conn = *it;
			if ((conn->m_lastSentTime + conn->m_tickFrequency) < GetAbsoluteTimeSeconds()) {
				conn->Tick();
			}

			if (conn->m_state == eConnectionState_Disconnected) { //This is needed in case a connection is DC'd without sending quit packet
				it = m_connections.erase(it);
				delete conn;
			}
			else
				++it;
		}

}

NetConnection* NetSession::AddConnection(const NetAddress& addr, bool isHost) {
	NetConnection* connection = new NetConnection();
	connection->m_owningSession = this;
	connection->m_netAddress = addr;

	if (isHost)
		connection->m_connectionIndex = 0;
	else
		connection->m_connectionIndex = 0xff;

	m_connections.push_back(connection);

	DevConsole::ConsolePrintf("%s %s %s %i", RGBA(0, 255, 0, 255), "Added connection to address ", addr.m_addr, "and port ", ntohs(addr.m_port));

	return connection;
}

NetConnection* NetSession::FindConnectionByNetAddress(NetAddress* addr) {
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		NetAddress na = nc->m_netAddress;

		bool isEqual = memcmp(na.m_addr, addr->m_addr, 16) == 0;

		if (isEqual && addr->m_port == na.m_port) {
			return nc;
		}
	}

	return nullptr;
}

NetConnection* NetSession::FindConnectionByIndex(int connID) {
	for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
		NetConnection* nc = *it;
		if (nc->m_connectionIndex == connID)
			return nc;
	}

	return nullptr;
}


