#pragma once
#include "MSocket.h"

//#define __COMPILE_WITH_SOCKET_SSL__

#ifdef __COMPILE_WITH_SOCKET_SSL__
#include <openssl/ssl.h>
class MSSLSocket : public MSocket
{
public:
	MSSLSocket();

	bool SSLInit();
	void SSLRelease();
protected:
	virtual void OnConnectSucc();

	virtual int32_t Send(const uint8_t* buffer, const uint32_t length); //loop
	virtual int32_t Recv(uint8_t* buffer, const uint32_t length, bool& isdisconnect); //loop	

private:
	SSL_CTX* m_ctx;
	SSL* m_ssl;
};
#else
class MSSLSocket : public MSocket
{
public:
	MSSLSocket() {}

	bool SSLInit() { return true; }
	void SSLRelease() {}
};
#endif
