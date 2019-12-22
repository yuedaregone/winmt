#pragma once
#include "MSocketIO.h"
#include "MSocket.h"
#include "MTimer.h"

class MHttpRequest;
class MHttpResponse;
class MHttpClient : public MSockMsgHandler, public MTimer
{
public:
	MHttpClient();
	~MHttpClient();

	MHttpRequest* CreateRequest(const char* url);

	void Request();

	virtual void OnUpdate();

	MHttpResponse* GetResponse() { return m_response; }

	void Reset();

	virtual void OnRecvData(SocketFD socket, MCycleBuffer* buffer);

	virtual void OnDisconnect(SocketFD socket);

protected:
	virtual void OnRecvEnd();

protected:
	bool m_finished = false;
	MSocket* m_socket;
	MHttpRequest* m_request;
	MHttpResponse* m_response;
};

