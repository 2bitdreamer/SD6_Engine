#include "NetSystem.hpp"
#include <corecrt_malloc.h>
#include "UDPSocket.hpp"
#include "NetSession.hpp"
#include "NetMessage.hpp"
#include "Utilities\DevConsole.hpp"
#include "NetConnection.hpp"
#include <functional>

NetSystem* NetSystem::GetInstance()
{
	static NetSystem* ns = new NetSystem();
	return ns;
}

bool NetSystem::Init()
{
	WSAData wsa_data;
	int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (error == 0) {
		NetMessageDefinition pingDef;
		pingDef.m_callback = PingCallback;
		pingDef.m_name = "ping";
		pingDef.m_options |= eNMO_Connectionless;
		NetMessage::RegisterMessageDefinition(0, pingDef);

		NetMessageDefinition pongDef;
		pongDef.m_callback = PongCallback;
		pongDef.m_name = "pong";
		pongDef.m_options |= eNMO_Connectionless;
		NetMessage::RegisterMessageDefinition(1, pongDef);

		NetMessageDefinition heartbeatDef;
		heartbeatDef.m_callback = HeartbeatCallback;
		heartbeatDef.m_name = "heartbeat";
		NetMessage::RegisterMessageDefinition(2, heartbeatDef);

		NetMessageDefinition ackDef;
		ackDef.m_callback = AckCallback;
		ackDef.m_name = "ack";
		NetMessage::RegisterMessageDefinition(3, ackDef);

		NetMessageDefinition joinRequest;
		joinRequest.m_callback = JoinRequestCallback;
		joinRequest.m_name = "joinrequest";
		joinRequest.m_options |= eNMO_Reliable;
		joinRequest.m_options |= eNMO_Connectionless;
		NetMessage::RegisterMessageDefinition(4, joinRequest);

		NetMessageDefinition joinAccept;
		joinAccept.m_callback = JoinAcceptCallback;
		joinAccept.m_name = "joinaccept";
		joinAccept.m_options |= eNMO_Reliable;
		NetMessage::RegisterMessageDefinition(5, joinAccept);

		NetMessageDefinition joinDeny;
		joinDeny.m_callback = JoinDenyCallback;
		joinDeny.m_name = "joindeny";
		joinDeny.m_options |= eNMO_Connectionless;
		NetMessage::RegisterMessageDefinition(6, joinDeny);

		NetMessageDefinition leave;
		leave.m_callback = LeaveCallback;
		leave.m_name = "leave";
		NetMessage::RegisterMessageDefinition(7, leave);

		NetMessageDefinition startTest;
		startTest.m_callback = StartTestCallback;
		startTest.m_name = "starttest";
		startTest.m_options |= eNMO_Reliable;
		NetMessage::RegisterMessageDefinition(8, startTest);


		NetMessageDefinition inOrderTest;
		inOrderTest.m_callback = InOrderTestCallback;
		inOrderTest.m_name = "inordertest";
		inOrderTest.m_options |= eNMO_Reliable;
		inOrderTest.m_options |= eNMO_InOrder;
		NetMessage::RegisterMessageDefinition(9, inOrderTest);

		NetMessageDefinition forceTest;
		forceTest.m_callback = ForceTestCallback;
		forceTest.m_name = "forcetest";
		forceTest.m_options |= eNMO_Reliable;
		NetMessage::RegisterMessageDefinition(10, forceTest);

		return true;
	}
	else {
		FATAL_ERROR("Could not setup WSA System");
		return false;
	}

	//Create netmessage definitions here
}

void NetSystem::Deinit()
{
	WSACleanup();
}

NetSystem::NetSystem()
{
	Init();

}

UDPSocket* NetSystem::CreateUDPSocket(NetPacketQueue *queue, short port)
{
	UDPSocket *sock = new UDPSocket(queue, port);
	return sock;
}

void NetSystem::FreeSocket(UDPSocket* sock)
{
	sock->Join();
	delete sock;
}

NetSession* NetSystem::CreateSession(short port)
{
	NetSession *newSession = new NetSession();

	NetConnection me;
	NetAddress* na = new NetAddress();

	size_t numAddr = 1;
	char szHostName[255];
	gethostname(szHostName, 255);
	NetAddressForHost(na, numAddr, AF_INET, szHostName, port, true);
	me.m_netAddress = *na;

	NetConnection* newConn = newSession->AddConnection(me.m_netAddress, false);
	newSession->m_me = newConn;
	newSession->m_me->m_connectionID = std::string("CHRISTIAN WALKER");

	UDPSocket *sock = CreateUDPSocket(&newSession->m_packetQueue, port);
	newSession->m_sockets.push_back(sock);

	m_networkSessions.push_back(newSession);

	return newSession;
}

void NetSystem::DestroySession(NetSession* s)
{
	for (auto it = m_networkSessions.begin(); it != m_networkSessions.end(); ) {
		NetSession* sess = *it;
		if (sess == s) {
			it = m_networkSessions.erase(it);
		}
		else {
			++it;
		}
	}

	free(s);
}

void PingCallback(net_sender_t &from, NetMessage* msg) {
	DevConsole::ConsolePrintf("%s", RGBA(0, 0, 255, 255), "Received ping");

	std::string test = std::string((char*)msg->GetBuffer());
	DevConsole::ConsolePrintf("%s %s", RGBA(0, 0, 255, 255), "Message Data: ", test.c_str());
}

void PongCallback(net_sender_t &from, NetMessage* msg) {
	DevConsole::ConsolePrintf("%s", RGBA(0, 255, 0, 255), "Received pong ");

	std::string test = std::string((char*)msg->GetBuffer());
	DevConsole::ConsolePrintf("%s %s", RGBA(0, 0, 255, 255), "Message Data: ", test.c_str());
}


void HeartbeatCallback(net_sender_t& from, NetMessage* msg) {
	DevConsole::ConsolePrintf("%s", RGBA(0, 0, 255, 255), "Received heartbeat!");
}


void InOrderTestCallback(net_sender_t& from, NetMessage* msg) {

}

void JoinAcceptCallback(net_sender_t& from, NetMessage* msg) {
	DevConsole::ConsolePrintf("%s", RGBA(0, 0, 255, 255), "Received join accept!");
	
	size_t byteAt = 0;
	unsigned char* messageData = msg->GetBuffer();

	uint8_t maxConnections = *(uint8_t*)(messageData + byteAt);
	byteAt += sizeof(uint8_t);

	uint8_t connID = *(uint8_t*)(messageData + byteAt);
	byteAt += sizeof(uint8_t);

	NetSystem* system = NetSystem::GetInstance();
	NetSession* gameSession = system->m_networkSessions[0];

	gameSession->m_me->m_connectionIndex = connID;
}

void ForceTestCallback(net_sender_t& from, NetMessage* msg) {

}

void JoinRequestCallback(net_sender_t& from, NetMessage* msg)
{
	DevConsole* devC = DevConsole::GetInstance();
	devC->ConsolePrintf("%s", RGBA(0, 0, 255, 255), "Received join request");

	NetSystem* system = NetSystem::GetInstance();
	NetSession* gameSession = system->m_networkSessions[0];


	NetConnection* connectionExists = gameSession->FindConnectionByNetAddress(&from.address);
	if (connectionExists)
		return;

	size_t byteAt = 0;

	uint8_t errorCode = 9;
	int sizeofID = -1;

	if (!gameSession->m_listening)
		errorCode = 0;
	else if (gameSession->m_connections.size() >= gameSession->m_maxConnections)
		errorCode = 1;
	else {
		unsigned char* messageData = msg->GetBuffer();
		std::string id = std::string((char*)messageData);

		sizeofID = id.size() + 1;
		byteAt += sizeofID;

		NetConnection* findConn = gameSession->FindConnectionByID(id);
		if (findConn)
			errorCode = 2;
	}

	if (errorCode != 9) {
		NetMessage* netMessage = new NetMessage();
		netMessage->m_messageDefinition = NetMessage::GetNetMessageDefinitionByName("joindeny");
		netMessage->SetMessageData((void*)&errorCode, sizeof(uint8_t));

		NetPacket* pack = new NetPacket();
		pack->CreateHeader();
		pack->AddMessage(netMessage);
		pack->UpdateNumMessages();
		gameSession->SendPacket(pack);

		delete netMessage;
	}
	else {
		
		NetMessage* netMessage = new NetMessage();
		netMessage->m_messageDefinition = NetMessage::GetNetMessageDefinitionByName("joinaccept");
		
		uint8_t firstValidConnectionIndex = 0;

		std::vector<NetConnection*>& connections = gameSession->m_connections;
		uint8_t maxConnections = gameSession->m_maxConnections;

		for (uint8_t index1 = 0; index1 < maxConnections; index1++) {
			bool foundAny = false;
			for (uint8_t index2 = 0; index2 < connections.size(); index2++) {
				int connectionIndex = connections[index2]->m_connectionIndex;
				if (connectionIndex == index1) {
					foundAny = true;
				}
			}
			if (!foundAny) {
				firstValidConnectionIndex = index1;
				break;
			}
		}

			
		byteAt = 0;

		unsigned char* buffer = netMessage->GetBuffer();

		uint8_t* maxConnectionsData = (uint8_t*)buffer + byteAt;
		*maxConnectionsData = maxConnections;
		byteAt += sizeof(uint8_t);

		uint8_t* joinerIndexData = (uint8_t*)buffer + byteAt;
		*joinerIndexData = firstValidConnectionIndex;
		byteAt += sizeof(uint8_t);

		uint8_t* hostIndexData = (uint8_t*)buffer + byteAt;
		*hostIndexData = gameSession->m_me->m_connectionIndex;
		byteAt += sizeof(uint8_t);

		std::string hostID = gameSession->m_me->m_connectionID;
		void* hostNameData = buffer + byteAt;
		size_t stringPlusNullSize = hostID.size() + 1;
		memcpy(hostNameData, hostID.c_str(), stringPlusNullSize);
		byteAt += stringPlusNullSize;

		uint8_t* invalidIndexData = buffer + byteAt;
		*invalidIndexData = 0xff;
		byteAt += sizeof(uint8_t);

		NetConnection* newConnection = gameSession->AddConnection(from.address, false);
		newConnection->m_owningSession = gameSession;
		newConnection->m_connectionIndex = firstValidConnectionIndex;
		newConnection->m_netAddress = from.address;
		newConnection->m_netAddress.m_port = newConnection->m_netAddress.m_port;

		netMessage->m_numBytesWritten = byteAt;
		gameSession->SendMsg(netMessage, newConnection);
	}
}


void StartTestCallback(net_sender_t& from, NetMessage* msg) {

	size_t byteAt = 0;
	unsigned char* messageData = msg->GetBuffer();
	uint32_t numInOrders = *(uint32_t*)(messageData + byteAt);
	byteAt += sizeof(uint32_t);

	from.connection->StartInorderTest(numInOrders);
}

void JoinDenyCallback(net_sender_t& from, NetMessage* msg)
{
	size_t byteAt = 0;
	unsigned char* messageData = msg->GetBuffer();
	uint8_t denyReason = *(uint8_t*)(messageData + byteAt);

	switch (denyReason) {
	case 0:
		DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Denied Join: NOT_JOINABLE");
		break;
	case 1:
		DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Denied Join: GAME_FULL");
		break;
	case 2:
		DevConsole::ConsolePrintf("%s", RGBA(255, 0, 0, 255), "Denied Join: ID_NOT_AVAILABLE");
		break;
	}

	NetSystem* system = NetSystem::GetInstance();
	NetSession* gameSession = system->m_networkSessions[0];

	if (from.connection)
		from.connection->m_state = eConnectionState_Disconnected;

	gameSession->RemoveConnection(gameSession->FindConnectionByNetAddress(&from.address));
}

void AckCallback(net_sender_t& from, NetMessage* msg) {
	DevConsole* devC = DevConsole::GetInstance();
	devC->ConsolePrintf("%s", RGBA(0, 0, 255, 255), "Received ack");


	NetConnection* connection = from.connection;
	if (connection == nullptr) {
		return;
	}

	size_t byteAt = 0;
	unsigned char* messageData = msg->GetBuffer();

	uint8_t numAcks = *(uint8_t*)(messageData + byteAt);
	byteAt += sizeof(uint8_t);

	for (int ack = 0; ack < numAcks; ack++) {
		uint16_t ackID = *(uint16_t*)(messageData + byteAt);
		byteAt += sizeof(uint16_t);

		ReliableTracker *tracker = connection->FindAndRemoveTracker(ackID);
		if (tracker != nullptr) {
			for (uint16_t reliable_id : tracker->m_reliableIDs) {
				connection->MarkReliableAsReceived(reliable_id);
			}
		}
	}
}

void LeaveCallback(net_sender_t& from, NetMessage* msg) {
	NetSystem* system = NetSystem::GetInstance();
	NetSession* gameSession = system->m_networkSessions[0];

	gameSession->RemoveConnection(from.connection);
}

void NetSystem::Tick()
{
	for (auto it = m_networkSessions.begin(); it != m_networkSessions.end(); ++it) {
		NetSession* ns = *it;
		ns->Tick();
	}
}