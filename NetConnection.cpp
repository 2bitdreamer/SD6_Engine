#include "NetConnection.hpp"
#include "NetMessage.hpp"
#include "NetSession.hpp"
#include "Utilities\Time.hpp"
#include "Utilities\DevConsole.hpp"



NetConnection::NetConnection() :
	m_lastSentTime(0.f),
	m_nextAck(0),
	m_tickFrequency(1.f),
	m_state(eConnectionState_Connected),
	m_timeLastSentHeartbeat(GetAbsoluteTimeSeconds()),
	m_session(nullptr),
	m_owningSession(nullptr),
	m_connectionIndex(0),
	m_nextReliableID(0),
	m_timeLastReceivedPacket(GetAbsoluteTimeSeconds()),
	m_nextOutgoingInOrderID(0),
	m_nextIncomingInOrderID(0)
{
}

void NetConnection::SendMsg(NetMessage *msg) {

	NetMessage* copy = new NetMessage(msg);
	NetMessageDefinition* defn = copy->m_messageDefinition;

	if ((defn->m_options & eNMO_Reliable) != 0) {
		m_unsentReliables.push_back(copy);
	} else {
		m_unsentUnreliables.push_back(copy);
	}

	delete msg;
}


void NetConnection::Tick()
{

	CheckForDisconnect();
	if (!IsConnected()) {
		return;
	}

	if (ShouldSendHeartbeat()) {
		SendHeartbeat();
	}

	CleanupTrackers();

	SendPacket();
}

void NetConnection::CheckForDisconnect()
{

	if (IsMe()) {
		return;
	}

	double current_time = GetAbsoluteTimeSeconds();
	double age = current_time - m_timeLastReceivedPacket;

	if (age > cConnection_TimeForDisconnect) {
		SetState(eConnectionState_Disconnected);
		DevConsole::ConsolePrintf("%s", RGBA(100, 255, 0, 255), "Disconnecting client....");
	}
}

void NetConnection::CleanupTrackers()
{
	while (!m_trackers.empty()) {
		ReliableTracker *tracker = m_trackers.back();

		if (tracker != nullptr) {
			double age = GetAbsoluteTimeSeconds() - tracker->m_timeCreated;
			if (((float)age) >= cTracker_MaxAge) {
				m_trackers.pop_back();
				delete tracker;
			}
			else {
				return;
			}
		}
	}
}

bool NetConnection::ShouldSendHeartbeat()
{
	if (m_state == eConnectionState_Disconnected)
		return false;

	if (IsMe()) {
		return false;
	}

	double current_time = GetAbsoluteTimeSeconds();
	double age = current_time - this->m_timeLastSentHeartbeat;

	if (age > cConnection_HeartbeatTime) {
		return true;
	}

	return false;
}

void NetConnection::SendHeartbeat()
{
	// However you send messages to your connection
	NetMessage* msg = new NetMessage(cMessage_Heartbeat);
	msg->m_messageDefinition = NetMessage::GetNetMessageDefinitionByName("heartbeat");
	SendMsg(msg);

	// Update clock
	m_timeLastSentHeartbeat = GetAbsoluteTimeSeconds();
}

bool NetConnection::IsMe()
{
	return m_owningSession->m_me == this;
}

void NetConnection::SendPacket()
{
	if (IsMe())
		return;


	// Get the ack for the packet, and add it.
	uint16_t ack = m_nextAck;
	NetPacket *packet = new NetPacket();

	packet->m_address = m_netAddress;
	packet->CreateHeader(m_owningSession->m_me->m_connectionIndex, m_nextAck);
	IncrementAck(m_nextAck);

	std::vector<NetMessage*> reliablesSentThisFrame;

	// Resend OLD reliables
	while (m_sentReliables.size() > 0) {
		NetMessage *msg = m_sentReliables.back();

		// If this got back to me, it will marked free - meaning it has been received
		if (m_sentReliableIDs.get(msg->m_reliableID) == false) {
			m_sentReliables.pop_back();
			delete msg;
		}
		else {
			// Resend if it has passed resend age
			double age = GetAbsoluteTimeSeconds() - msg->m_lastSentTime;
			if (((float)age) >= cNetMessage_ResendTime) {
				if (packet->AddMessage(msg)) {
					m_sentReliables.pop_back();
					reliablesSentThisFrame.push_back(msg);
				}
				else {
					break;
				}
			}
			else {
				break;
			}
		}
	}



	if (!m_inOrderTestMessages.empty()) {
		NetMessage* messageToSend = m_inOrderTestMessages.front();
		m_unsentReliables.push_back(messageToSend);
		m_inOrderTestMessages.erase(m_inOrderTestMessages.begin());
	}

	// Send Unsent Reliables
	while (!m_unsentReliables.empty()) {
		NetMessage *msg = m_unsentReliables.back();
		uint16_t reliableID = m_nextReliableID;

		// Is the reliable ID free to be used yet? 
		// If not, stop sending new traffic
		if (!CanUseReliableID(reliableID)) {
			break;
		}

		msg->m_reliableID = reliableID;
		if (packet->AddMessage(msg)) {
			IncrementReliableID(m_nextReliableID);
			m_unsentReliables.pop_back();
			reliablesSentThisFrame.push_back(msg);
			m_sentReliableIDs.set(msg->m_reliableID);
		}
		else {
			break; // stop - no more room.
		}
	}

	// TODO:  Add ACK information

	SendAllReceivedAcks(packet);

	// Add Unreliable Traffic
	for(NetMessage* msg : m_unsentUnreliables) {
		packet->AddMessage(msg); // we don't care if we fail for unreliables
	} 

	if (packet->m_msgCount > 0) {
		packet->UpdateNumMessages();
		m_owningSession->SendPacket(packet);

		for (NetMessage* msg : m_unsentUnreliables) {
			delete msg;
		}

		m_unsentUnreliables.clear();

	}
	else {
		delete packet;
	}

	// Track which reliables were sent with this packet.
	if (reliablesSentThisFrame.size() > 0) {
		ReliableTracker *tracker = new ReliableTracker(ack, reliablesSentThisFrame.size());
		while (reliablesSentThisFrame.size() > 0) {
			NetMessage *msg = reliablesSentThisFrame.back();
			reliablesSentThisFrame.pop_back();

			tracker->m_reliableIDs.push_back(msg->m_reliableID);
			msg->m_lastSentTime = GetAbsoluteTimeSeconds();
			m_sentReliables.push_back(msg);
		}
		m_trackers.push_back(tracker);
	}

// 	if (m_outgoingMessages.size() > 0) {
// 
// 		NetPacket* packet = new NetPacket();
// 
// 		packet->m_address = m_netAddress;
// 
// 		while (m_outgoingMessages.size() > 0) {
// 			NetMessage* netMsg;
// 			m_outgoingMessages.dequeue(&netMsg);
// 			bool messageFit = packet->AddMessage(netMsg);
// 
// 			if (!messageFit)
// 				break;
// 		}
// 
// 		packet->UpdateNumMessages();
// 
// 		m_owningSession->SendPacket(packet);
// 		m_lastSentTime = (float)GetAbsoluteTimeSeconds();
	//}
}

bool NetConnection::CanUseReliableID(uint16_t reliable_id)
{
	int toCheck = reliable_id - cConnection_ReliableWindowSize;
	if (toCheck < 0)
		toCheck += cConnection_MaxReliableID;
	
	bool set = m_sentReliableIDs.get(toCheck);
	if (set)
		return false;
	else
		return true;
}

// Call this every time you receive a packet from this 
// connection.
void NetConnection::ProcessPacket(NetPacket* packet)
{
	m_timeLastReceivedPacket = GetAbsoluteTimeSeconds();
	(void)packet;
}

bool NetConnection::IsConnected()
{
	return m_state == eConnectionState_Connected;
}

void NetConnection::IncrementAck(uint16_t& ack)
{
	++m_nextAck;
	(void)ack;
}

void NetConnection::IncrementReliableID(uint16_t nextRelID)
{
	++m_nextReliableID;
	(void)nextRelID;
}

void NetConnection::SendAllReceivedAcks(NetPacket * packet)
{
	if (m_receivedAcks.empty())
		return;

	unsigned char receivedAckCount = (unsigned char)min(0xff, m_receivedAcks.size());
	NetMessage* ackMessage = new NetMessage();
	ackMessage->m_messageDefinition = NetMessage::GetNetMessageDefinitionByName("ack");
	ackMessage->Write<unsigned char>(receivedAckCount);

	for (int i = 0; i < receivedAckCount; ++i) {
		ackMessage->Write<uint16_t>(m_receivedAcks[i]);
	}

	m_receivedAcks.clear();

	packet->AddMessage(ackMessage);

	delete ackMessage;
}

void NetConnection::TrackAck(uint16_t ack)
{
	FATAL_ASSERT(!IsMe());
	m_receivedAcks.push_back(ack);
}


ReliableTracker* NetConnection::FindAndRemoveTracker(uint16_t ackID)
{
	for (auto it = m_trackers.begin(); it != m_trackers.end(); ) {
		ReliableTracker* track = *it;
		if (track->m_packetAck == ackID) {
			it = m_trackers.erase(it);
			return track;
		}
		else
			++it;
	}
	return nullptr;
}


void NetConnection::StartInorderTest(uint32_t num) {

	if (!m_inOrderTestMessages.empty())
		return;

	for (uint32_t index = 0; index < num; index++) {
		NetMessage* msg = new NetMessage(9);
		msg->m_messageDefinition = NetMessage::GetNetMessageDefinitionByName("inordertest");
		msg->Write<uint32_t>(index);
		m_inOrderTestMessages.push_back(msg);
	}
}

void NetConnection::StartForceTest(uint32_t num) {
	NetMessage* forceTestMessage = new NetMessage();
	forceTestMessage->m_messageDefinition = NetMessage::GetNetMessageDefinitionByName("forcetest");
	forceTestMessage->SetMessageData((void*)&num, sizeof(uint32_t));
	SendMsg(forceTestMessage);
}

void NetConnection::MarkReliableAsReceived(uint16_t reliable_id)
{
	m_sentReliableIDs.unset(reliable_id);
}

NetConnection::~NetConnection()
{
}

