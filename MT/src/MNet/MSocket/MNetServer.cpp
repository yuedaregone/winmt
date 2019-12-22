#include "MNetServer.h"

void MNetServer::Init()
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
	int ret = m_socket.OpenSocketAsServer("127.0.0.1", 60320);
	if (ret != S_OK)
	{
		Destroy();
	}
}

void MNetServer::Destroy()
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

bool MNetServer::SendMsg(const uint8_t * msg, uint32_t size)
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

void MNetServer::MakeMessage(SocketFD socket, MCycleBuffer* buffer)
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

bool MNetServer::CheckMsgHead(uint8_t * buffer, MSG_HEAD* head)
{
	memcpy(head, buffer, sizeof(MSG_HEAD));
	if (head->tag == MSG_TAG)
	{
		return true;
	}
	return false;
}

void MNetServer::OnRecvMsgBody(SocketFD socket, uint8_t * buffer, uint32_t size)
{
	// msg body

	char* out = new char[size + 1];
	memcpy(out, buffer, size);
	out[size] = '\0';
	printf(out);
	printf("\n");
	delete[] out;
}

void MNetServer::OnRecvData(SocketFD socket, MCycleBuffer* buffer)
{
	//.................//
	MakeMessage(socket, buffer);
}

void MNetServer::OnAcceptClient()
{
	int ret = m_socket.AcceptClient();
	if (ret != -1)
	{
		MSocketIO* sockIO = new MSocketIO(ret);
		sockIO->RegisterMsgHandler(this);
		m_socketIOs.push_back(sockIO);
	}
}

void MNetServer::CheckSocketState()
{
	std::vector<MSocketIO*>::iterator it = m_socketIOs.begin();
	for (; it != m_socketIOs.end(); )
	{
		uint8_t state = (*it)->GetState();
		if (state != S_OK)
		{
			it = m_socketIOs.erase(it);
		}
		else
		{
			it++;
		}
	}
}

void MNetServer::Update()
{
	OnAcceptClient();

	//m_socket.Update();

	std::vector<MSocketIO*>::iterator it = m_socketIOs.begin();
	for (; it != m_socketIOs.end(); ++it)
	{
		(*it)->Update();
	}

	CheckSocketState();
}