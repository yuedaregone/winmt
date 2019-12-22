#pragma once
#include "MNetDef.h"
#include "MSocket.h"
#include "MCycleBuffer.h"
#include <vector>

class MNetServer : public MSockMsgHandler
{
public:
	void Init();
	void Update();
	void Destroy();
public:
	bool SendMsg(const uint8_t* msg, uint32_t size);
	virtual void OnRecvData(SocketFD socket, MCycleBuffer* buffer);

private:	
	void MakeMessage(SocketFD socket, MCycleBuffer* buffer);
	bool CheckMsgHead(uint8_t* buffer, MSG_HEAD* head);
	void OnRecvMsgBody(SocketFD socket, uint8_t* buffer, uint32_t size);

	void OnAcceptClient();
	void CheckSocketState();
private:
	MSocket m_socket;
	std::vector<MSocketIO*> m_socketIOs;
};
