#include "MHttpClient.h"
#include "MSSLSocket.h"
#include <iostream>
#include <stdio.h>
#include "MHttpRequest.h"
#include "MHttpResponse.h"


MHttpClient::MHttpClient()
	: m_request(NULL)
	, m_finished(false)
	, m_response(NULL)
{
#ifdef _WIN32
	WSAData data;
	int result = ::WSAStartup(0x0202, &data);
	if (result != 0)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Error:Start up socket! %d\n", errorret);
		return;
	}
#endif
}

MHttpClient::~MHttpClient()
{
#ifdef _WIN32
	int result = ::WSACleanup();
	if (result != 0)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Error:Start up socket! %d\n", errorret);
	}
#endif
	if (m_response != NULL)
	{
		delete m_response;
		m_response = NULL;
	}
	if (m_socket != NULL)
	{
		if (m_request && m_request->SSL())
		{
			MSSLSocket* sslSocket = dynamic_cast<MSSLSocket*>(m_socket);
			if (sslSocket)
			{
				sslSocket->SSLRelease();
			}
			delete m_request;
			m_request = NULL;
		}

		delete m_socket;
		m_socket = NULL;
	}
}

void MHttpClient::Reset()
{
	m_finished = true;
	if (m_response == NULL)
	{
		m_response = new MHttpResponse();
	}
	else
	{
		m_response->Clear();
	}
}

MHttpRequest * MHttpClient::CreateRequest(const char * url)
{
	m_request = new MHttpRequest(url);
	return m_request;
}

void MHttpClient::Request()
{
	if (m_finished || m_request == NULL) return;
	Reset();

	if (m_request->SSL())
	{
		MSSLSocket* sslSocket = new MSSLSocket();
		m_socket = sslSocket;
		sslSocket->SSLInit();
	}
	else
	{
		m_socket = new MSocket();
	}

	m_socket->OpenSocketAsClient();

	if (m_socket->Connect(m_request->GetIP(), m_request->GetPort()) != (int)S_OK)
	{
		return;
	}
	
	m_socket->RegisterMsgHandler(this);
	const uint8_t* sendStr = NULL;
	uint32_t dataLen = m_request->GetSendData(&sendStr);
	m_socket->SendData(sendStr, dataLen);
}

void MHttpClient::OnUpdate()
{
	if (!m_finished) return;

	m_socket->Update();
}

void MHttpClient::OnRecvData(SocketFD socket, MCycleBuffer* buffer)
{
	uint8_t buff[BUFFER_SIZE];

	uint32_t dataSize = buffer->Size();
	uint32_t nRead = 0;
	do 
	{
		uint32_t nr = buffer->Read(buff, BUFFER_SIZE);
		if (nr > 0)
		{
			m_response->OnResponseData(buff, nr);
			nRead += nr;
		}			
		else
		{
			break;
		}
	} while (nRead > dataSize);
	
	if (m_response->Finished())
	{
		OnRecvEnd();
	}
}

void MHttpClient::OnDisconnect(SocketFD socket)
{
	OnRecvEnd();
}

void MHttpClient::OnRecvEnd()
{
	m_finished = false;

	printf("Finish\n");	
	m_socket->CloseSocket();

	std::string reqFile = m_request->GetRequestFile();
	
	const char* file = reqFile.c_str();
	FILE* fp = fopen(file, "wb+");
	if (fp != NULL)
	{
		fwrite(m_response->GetDataPtr(), m_response->GetContentSize(), 1, fp);
		fclose(fp);
	}	
}
