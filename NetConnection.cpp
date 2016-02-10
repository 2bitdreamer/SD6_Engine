#include "NetConnection.hpp"
#include "NetMessage.hpp"
#include "NetSession.hpp"



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

	NetPacket* packet = new NetPacket();

	packet->m_address = m_netAddress;
	NetMessage msg(NetMessage::GetNetMessageDefinitionByName("ping")->m_id);

	for (int index = 0; index < m_outgoingMessages.size(); index++) {
		NetMessage* netMsg;
		m_outgoingMessages.dequeue(&netMsg);
		packet->AddMessage(msg);
	}

	packet->UpdateNumMessages();

	m_owningSession->SendPacket(packet);

}

NetConnection::~NetConnection()
{
}
