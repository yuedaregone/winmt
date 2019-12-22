#include "MSSLSocket.h"

#ifdef __COMPILE_WITH_SOCKET_SSL__
#include "MNetApi.h"
#include "MSocketIO.h"
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#pragma comment(lib, "crypt32.lib") 
#pragma comment(lib, "libcrypto.lib") 
#pragma comment(lib, "libssl.lib") 

MSSLSocket::MSSLSocket()
	: m_ssl(NULL)
	, m_ctx(NULL)
{	
}

bool MSSLSocket::SSLInit()
{	
	SSL_library_init();
	SSL_load_error_strings();

	m_ctx = SSL_CTX_new(SSLv23_client_method());
	if (m_ctx == NULL)
	{
		//--ERROR 
		return false;
	}
	m_ssl = SSL_new(m_ctx);
	if (m_ssl == NULL)
	{
		//--ERROR
		return false;
	}
	return true;
}

void MSSLSocket::SSLRelease()
{
	if (m_ssl != NULL)
	{
		SSL_shutdown(m_ssl);
		SSL_free(m_ssl);
		m_ssl = NULL;
	}
	if (m_ctx != NULL)
	{
		SSL_CTX_free(m_ctx);
		m_ctx = NULL;
	}
}

void MSSLSocket::OnConnectSucc()
{
	if (!SSL_set_fd(m_ssl, (int)m_socket))
	{
		LogError("SSL_set_fd Error!");
		return;
	}
	if (!SSL_connect(m_ssl))
	{
		LogError("SSL_connect Error!");
		return;
	}
}

static int32_t openssl_select_fd(SSL* ssl, int32_t fd, double maxtime, int32_t wait_for)
{
	if (SSL_pending(ssl))
		return 1;
	return select_fd(fd, maxtime, wait_for);
}

int32_t MSSLSocket::Send(const uint8_t* buffer, const uint32_t length)
{
	int sendBytes = 0;
	while (sendBytes < (int)length)
	{
		int32_t ret = openssl_select_fd(m_ssl, m_socket, 0.01, WAIT_FOR_WRITE);
		if (ret == 0)
		{
			break;
		}
		else if (ret > 0)
		{
			int len = ::SSL_write(m_ssl, buffer + sendBytes, length - sendBytes);
			//int len = ::send(m_socket, (const char*)(buffer + sendBytes), length - sendBytes, 0);
			if (len > 0)
			{
				sendBytes += len;
			}
			else
			{
				OnSocketError();
				break;
			}
		}
		else if (ret < 0)
		{
			OnSocketError();
			break;
		}
	}
	return sendBytes;
}

int32_t MSSLSocket::Recv(uint8_t* buffer, const uint32_t length, bool& isdisconnect)
{
	int recvBytes = 0;
	while (recvBytes < (int)length)
	{
		int32_t ret = openssl_select_fd(m_ssl, m_socket, 0.01, WAIT_FOR_READ);
		if (ret == 0)
		{
			break;
		}
		else if (ret > 0)
		{
			int len = SSL_read(m_ssl, buffer + recvBytes, length - recvBytes);
			//int len = ::recv(m_socket, (char*)(buffer + recvBytes), length - recvBytes, 0);
			if (len > 0)
			{
				recvBytes += len;
			}
			else
			{
				if (len == 0)
				{
					isdisconnect = true;
				}
				OnSocketError();
				break;
			}
		}
		else
		{
			OnSocketError();
			break;
		}
	}
	return recvBytes;
}

#endif
