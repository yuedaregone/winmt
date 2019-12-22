#pragma once
#include "MNetDef.h"
#include "MSocket.h"
#include "MCycleBuffer.h"

class MNetClient : public MSockMsgHandler
{
public:
	void Init();
	void Update();
	void Destroy();
public:
	void Connet(const char* remoteIP, uint16_t port);

	bool SendMsg(const uint8_t* msg, uint32_t size);

	virtual void OnRecvData(SocketFD socket, MCycleBuffer* buffer);

private:
	void MakeMessage(SocketFD socket, MCycleBuffer* buffer);
	bool CheckMsgHead(uint8_t* buffer, MSG_HEAD* head);
	void OnRecvMsgBody(SocketFD socket, uint8_t* buffer, uint32_t size);
private:
	MSocket m_socket;	
};

