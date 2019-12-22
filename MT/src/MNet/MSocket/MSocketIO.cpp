#include "MSocketIO.h"
#include "MNetApi.h"

void MSocketIO::SendData(const uint8_t* msg, uint32_t size)
{
	if (m_writeBuffer.Space() < size)
	{
		LogError("Send Failed! Write Buff is Full!");
		return;
	}

	uint32_t nWrite = 0;	
	do
	{
		nWrite += m_writeBuffer.Write((uint8_t *)(msg + nWrite), size - nWrite);
	} while (nWrite < size);
}

void MSocketIO::CloseSocket()
{
#ifdef _WIN32
	::closesocket(m_socket);
#else
	close(m_socket);
#endif 
	m_readBuffer.Reset();
	m_writeBuffer.Reset();
}

void MSocketIO::OnReadData()
{
	bool disconnect = false;
	int32_t nRecv = Recv(m_recvBuffer, BUFFER_SIZE, disconnect);
	if (nRecv == 0)
	{
		return;
	}
	else if (nRecv > 0)
	{
		uint32_t nWrite = 0;
		do
		{
			nWrite += m_readBuffer.Write(m_recvBuffer + nWrite, nRecv - nWrite);
			if (m_msgHandler != NULL)
				m_msgHandler->OnRecvData(m_socket, &m_readBuffer);
		} while (nWrite < (uint32_t)nRecv);
	}
	if (disconnect)
	{
		m_state = SK_DISCONNECT;
		LogError("Net disconnect!");
		if (m_msgHandler != NULL)
			m_msgHandler->OnDisconnect(m_socket);
		return;
	}	
}

void MSocketIO::OnSendData()
{
	if (m_writeBuffer.Size() == 0)
		return;

	uint32_t nWrite = m_writeBuffer.PeekRead(m_sendBuffer, BUFFER_SIZE);
	if (nWrite == 0)
	{
		return;
	}	
	uint32_t nSend = (uint32_t)Send(m_sendBuffer, nWrite);
	if (nSend > 0)
	{
		m_writeBuffer.PeekSetRead(nSend);
	}
}

void MSocketIO::Update()
{
	OnSendData();
	if (m_msgHandler != NULL)
		OnReadData();	
}

int32_t MSocketIO::Send(const uint8_t* buffer, const uint32_t length)
{
	int sendBytes = 0;
	while (sendBytes < (int)length)
	{
		int32_t ret = select_fd(m_socket, 0.01, WAIT_FOR_WRITE);
		if (ret == 0)
		{
			break;
		}
		else if (ret > 0)
		{
			int len = ::send(m_socket, (const char*)(buffer + sendBytes), length - sendBytes, 0);
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

void MSocketIO::OnSocketError()
{
	int errorret = 0;
#ifdef _WIN32		
	errorret = GET_ERROR_CODE;
	if (errorret == WSAEWOULDBLOCK)
#else
	errorret = GET_ERROR_CODE;
	if (errorret == EINPROGRESS)
#endif
	{
		LogError("IO Busy\n");		
	}
	else
	{
		m_state = SK_UNKNOWN;
		LogError("Socket Error: %d\n", errorret);
	}
}

int32_t MSocketIO::Recv(uint8_t* buffer, const uint32_t length, bool& isdisconnect)
{
	int recvBytes = 0;
	while (recvBytes < (int)length)
	{
		int32_t ret = select_fd(m_socket, 0.01, WAIT_FOR_READ);
		if (ret == 0)
		{
			break;
		}
		else if (ret > 0)
		{
			int len = ::recv(m_socket, (char*)(buffer + recvBytes), length - recvBytes, 0);
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
