#pragma once
#include "MNetDef.h"
#include "MCycleBuffer.h"

class MSockMsgHandler
{
public:
	virtual ~MSockMsgHandler() {}
	virtual void OnRecvData(SocketFD socket, MCycleBuffer* buffer) = 0;
	virtual void OnDisconnect(SocketFD socket) {}
};

class MSocketIO
{
public:
	MSocketIO(SocketFD socket) : m_msgHandler(NULL), m_socket(socket), m_state(S_OK) {}
	MSocketIO() : m_msgHandler(NULL), m_state(S_OK) {};
	virtual ~MSocketIO() {}

	virtual void Update();

	void SendData(const uint8_t* msg, uint32_t size);
	
	virtual void CloseSocket();

	uint8_t GetState() { return m_state; }

	uint32_t WriteSpace() { return m_writeBuffer.Space(); }

	void RegisterMsgHandler(MSockMsgHandler* handler) { m_msgHandler = handler; }
	void UnregisterMsgHandler() { m_msgHandler = NULL; }
protected:
	void OnReadData();
	void OnSendData();
	void OnSocketError();

	virtual int32_t Send(const uint8_t* buffer, const uint32_t length); //loop
	virtual int32_t Recv(uint8_t* buffer, const uint32_t length, bool& isdisconnect); //loop	
protected:
	MSockMsgHandler* m_msgHandler;
	SocketFD m_socket;
	MCycleBuffer m_readBuffer;
	MCycleBuffer m_writeBuffer;	

	uint8_t m_recvBuffer[BUFFER_SIZE];
	uint8_t m_sendBuffer[BUFFER_SIZE];

	int8_t m_state;
};
