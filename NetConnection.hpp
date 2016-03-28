#pragma once
#include <stdint.h>
#include <vector>
#include "NetAddress.hpp"
#include "Utilities\ThreadSafeQueue.hpp"
#include "BitArray.hpp"
#include <xfunctional>
class NetMessage;
class NetSession;
class NetPacket;
class NetConnection;

double const cConnection_TimeForDisconnect = 30.0;
double const cConnection_HeartbeatTime = 2.5;
int const cConnection_ReliableWindowSize = 4096;
int const cConnection_MaxReliableID = 0xffff;
float const cNetMessage_ResendTime = 0.1f;

#define cMessage_Ping 			0
#define cMessage_Pong			1 
#define cMessage_Heartbeat		2

float const cTracker_MaxAge = 2.5f;

// This is usually in netaddress.hpp
struct net_sender_t
{
	NetSession *session;
	NetAddress address;
	NetConnection *connection;
};

enum eConnectionState
{
	eConnectionState_Connected,			// Starts in this state.
	eConnectionState_Disconnected,
};

class ReliableTracker
{
public:
	ReliableTracker(uint16_t ack, size_t count)
	{
		m_timeCreated = GetCurrentTime();
		m_packetAck = ack;

		m_reliableIDs.reserve(count);
	}

	double m_timeCreated; // when was this tracker created
	uint16_t m_packetAck; // Packet this is for.
	std::vector<uint16_t> m_reliableIDs; // Reliables IDs sent with this ack.
};

class NetConnection
{
public:
	NetConnection();
	~NetConnection();

	void SendMsg(NetMessage *msg);
	void Tick();
	void CheckForDisconnect();
	void CleanupTrackers();
	bool ShouldSendHeartbeat();
	void SendHeartbeat();
	void SetState(eConnectionState state) { m_state = state; }
	bool IsMe();
	void SendPacket();
	bool CanUseReliableID(uint16_t reliable_id);
	void ProcessPacket(NetPacket* packet);
	bool IsConnected();
	void IncrementAck(uint16_t& m_nextAck);
	void IncrementReliableID(uint16_t m_nextReliableID);
	void SendAllReceivedAcks(NetPacket * packet);
	void TrackAck(uint16_t ack);
	ReliableTracker* FindAndRemoveTracker(uint16_t ackID);
	void StartInorderTest(uint32_t num);
	void StartForceTest(uint32_t num);
	void MarkReliableAsReceived(uint16_t reliable_id);
public:
	ThreadSafeQueue<NetMessage*> m_outgoingMessages;

	float m_lastSentTime;
	float m_tickFrequency;
	double m_timeLastReceivedPacket;
	double m_timeLastSentHeartbeat;
	eConnectionState m_state;

	uint16_t m_nextAck;
	NetSession *m_session;

	uint16_t m_nextReliableID;
	TBitArray<cConnection_MaxReliableID> m_receivedReliableID;

	TBitArray<cConnection_MaxReliableID> m_sentReliableIDs;
	TBitArray<cConnection_MaxReliableID> m_receivedReliableIDs;

	std::vector<NetMessage*> m_unsentReliables;
	std::vector<NetMessage*> m_sentReliables;
	std::vector<NetMessage*> m_unsentUnreliables;
	std::vector<uint16_t> m_receivedAcks;
	
	std::vector<ReliableTracker*> m_trackers;

	unsigned char m_connectionIndex;
	std::string m_connectionID;

	uint16_t m_nextOutgoingInOrderID;
	uint16_t m_nextIncomingInOrderID;

	NetAddress m_netAddress;
	NetSession* m_owningSession;

	std::vector<NetMessage*> m_inOrderTestMessages;

	std::priority_queue<int, std::vector<uint16_t>, std::greater<uint16_t> > m_messages;
};

