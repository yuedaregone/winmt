#pragma once
#include "MSocketIO.h"

class MSocket : public MSocketIO
{
public:
	int OpenSocketAsClient(const char* localIP = NULL);
	int OpenSocketAsServer(const char* localIP, uint16_t port);
	void CloseSocket();

	int Connect(const char* remoteIP, uint16_t port);	

	int AcceptClient(); //loop

	bool ConnectOK() { return m_state == SK_SUCCESS; }

	virtual void Update();

protected:

	void Clear();

	void CheckConnect();

	virtual void OnConnectSucc();
private:	
	char m_socketIP[IP_MAX_LEN];
	uint16_t m_port;
	SocketType m_socketType;		
};
