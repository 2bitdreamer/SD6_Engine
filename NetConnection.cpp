#include "NetConnection.hpp"
#include "NetMessage.hpp"



NetConnection::NetConnection() :
	m_lastSentTime(0.f),
	m_nextAckID(0)
{
}

void NetConnection::SendMessage(NetMessage *msg) {

	NetMessage* netMessageCopy = new NetMessage(msg);
	delete msg;

	m_outgoingMessages.enqueue(netMessageCopy);
}


NetConnection::~NetConnection()
{
}
