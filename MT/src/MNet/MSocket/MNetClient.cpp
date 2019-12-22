#include "MNetClient.h"
#include "MCycleBuffer.h"

void MNetClient::Init()
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
	int ret = m_socket.OpenSocketAsClient();
	if (ret != S_OK)
	{
		Destroy();
	}
}

void MNetClient::Destroy()
{
#ifdef _WIN32
	int result = ::WSACleanup();
	if (result != 0)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Error:Start up socket! %d\n", errorret);
	}
#endif
}

void MNetClient::Connet(const char * remoteIP, uint16_t port)
{
	if (m_socket.Connect(remoteIP, port) == (int)S_OK)
	{
		m_socket.RegisterMsgHandler(this);
	}	
}

bool MNetClient::SendMsg(const uint8_t * msg, uint32_t size)
{
	MSG_HEAD head;
	head.tag = MSG_TAG;
	head.length = size;

	if (m_socket.WriteSpace() < sizeof(MSG_HEAD) + size)
	{
		LogError("Send Failed! Write Buff is Full!");
		return false;
	}
	m_socket.SendData((uint8_t *)&head, sizeof(MSG_HEAD));
	m_socket.SendData(msg, size);
	return true;
}

void MNetClient::MakeMessage(SocketFD socket, MCycleBuffer* buffer)
{
	if (buffer->Size() == 0)
		return;

	uint8_t makeMsgBuff[BUFFER_SIZE];

	int nRead = buffer->PeekRead(makeMsgBuff, BUFFER_SIZE);
	if (nRead == 0)
	{
		return;
	}
	int readIdx = 0;
	while (readIdx < nRead)
	{
		MSG_HEAD msgHead;
		if (!CheckMsgHead(makeMsgBuff + readIdx, &msgHead))
		{			
			LogError("Error! Recv data");
			return;
		}
		if (msgHead.length + sizeof(MSG_HEAD) + readIdx > (uint32_t)nRead)
		{
			break;
		}

		OnRecvMsgBody(socket, makeMsgBuff + sizeof(MSG_HEAD), msgHead.length);

		readIdx += msgHead.length + sizeof(MSG_HEAD);
	}
	buffer->PeekSetRead(readIdx);
}

bool MNetClient::CheckMsgHead(uint8_t * buffer, MSG_HEAD* head)
{
	memcpy(head, buffer, sizeof(MSG_HEAD));
	if (head->tag == MSG_TAG)
	{
		return true;
	}
	return false;
}

void MNetClient::OnRecvMsgBody(SocketFD socket, uint8_t * buffer, uint32_t size)
{
	// msg body

	char* out = new char[size + 1];
	memcpy(out, buffer, size);
	out[size] = '\0';
	printf(out);
	printf("\n");
	delete[] out;
}

void MNetClient::OnRecvData(SocketFD socket, MCycleBuffer* buffer)
{
	//.................//
	MakeMessage(socket, buffer);
}

void MNetClient::Update()
{
	m_socket.Update();
}


