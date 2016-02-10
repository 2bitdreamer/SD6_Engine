#include "NetConnection.hpp"
#include "NetMessage.hpp"
#include "NetSession.hpp"
#include "Utilities\Time.hpp"



NetConnection::NetConnection() :
	m_lastSentTime(0.f),
	m_nextAckID(0),
	m_tickFrequency(5.f)
{
}

void NetConnection::SendMessage(NetMessage *msg) {

	NetMessage* netMessageCopy = new NetMessage(msg);
	delete msg;

	m_outgoingMessages.enqueue(netMessageCopy);
}


void NetConnection::Tick()
{
	//Construct packet
	//put as many messages as you can in packet
	//Send

	if (m_outgoingMessages.size() > 0) {

		NetPacket* packet = new NetPacket();

		packet->m_address = m_netAddress;;

		for (int index = 0; index < m_outgoingMessages.size(); index++) {
			NetMessage* netMsg;
			m_outgoingMessages.dequeue(&netMsg);
			packet->AddMessage(netMsg);
		}

		packet->UpdateNumMessages();

		m_owningSession->SendPacket(packet);
		m_lastSentTime = (float)GetAbsoluteTimeSeconds();
	}

}

NetConnection::~NetConnection()
{
}
