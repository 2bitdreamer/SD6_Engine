#include "NetConnection.hpp"



NetConnection::NetConnection() :
	m_lastSentTime(0.f),
	m_nextAckID(0)
{
}


NetConnection::~NetConnection()
{
}
