#include "MSocket.h"
#include "MNetApi.h"

void MSocket::Clear()
{
	m_socket = InvalideSocket;
	memset(m_socketIP, 0, IP_MAX_LEN);
	m_port = 0;
	m_socketType = SocketUnknown;
	m_state = SK_NOACTIVATE;
	m_msgHandler = NULL;
}

int MSocket::OpenSocketAsClient(const char* localIP)
{
	Clear();

	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == InvalideSocket)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Socket Error: Create Client Socket Error! %d\n", errorret);
		return S_ERROR;
	}
	if (localIP != NULL)
	{
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(localIP);
		int32_t ret = ::bind(m_socket, (struct sockaddr *)&addr, sizeof(addr));
		if (0 != ret)
		{
			int errorret = GET_ERROR_CODE;
			LogError("Socket Error: Bind Client Socket Error! %d\n", errorret);
			CloseSocket();
			return S_ERROR;
		}
	}
	set_non_block(m_socket);
	m_socketType = SocketCommunication;
	m_state = SK_CREATED;
	return S_OK;
}

int MSocket::OpenSocketAsServer(const char * localIP, uint16_t port)
{
	Clear();

	m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == InvalideSocket)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Socket Error: Create Server Socket Error! %d\n", errorret);
		return S_ERROR;
	}

	int32_t op = 1;
	int32_t ret = ::setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&op, sizeof(op));
	if (ret != 0)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Socket Error: Set Socket Opt Error! %d\n", errorret);
		return S_ERROR;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	if ((NULL == localIP) || (strlen(localIP) == 0))
	{
		addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
	}
	else
	{
		addr.sin_addr.s_addr = inet_addr(localIP);
	}
	addr.sin_port = ::htons(port);

	ret = ::bind(m_socket, (struct sockaddr*)&addr, sizeof(addr));
	if (ret != 0)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Socket Error: Bind Server Socket Error! %d\n", errorret);
		CloseSocket();
		return S_ERROR;
	}

	ret = ::listen(m_socket, SOMAXCONN);
	if (ret != 0)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Socket Error: Listen Socket Error! %d\n", errorret);
		CloseSocket();
		return S_ERROR;
	}
	strcpy(m_socketIP, localIP);
	m_port = port;

	set_non_block(m_socket);
	m_socketType = SocketListen;
	m_state = SK_CREATED;
	return S_OK;
}

int MSocket::AcceptClient()
{
	struct sockaddr_in addr;
	int size = (int)sizeof(addr);
	int ret = ::accept(m_socket, (struct sockaddr*)&addr, &size);
	if (ret == SOCKET_ERROR)
	{
		int errorret = 0;
#ifdef _WIN32		
		errorret = GET_ERROR_CODE;
		if (errorret != WSAEWOULDBLOCK)
#else
		errorret = GET_ERROR_CODE;
		if (errorret != EINPROGRESS)
#endif
		{
			LogError("Socket Error: Connect Error %d\n", errorret);
			CloseSocket();
			return S_ERROR;
		}
		return S_ERROR;
	}
	return ret;
}

int MSocket::Connect(const char* remoteIP, uint16_t port)
{
	if (remoteIP == NULL)
	{
		LogError("Socket Error: Remote IP is NULL");
		return S_ERROR;
	}
	strcpy(m_socketIP, remoteIP);
	m_port = port;

	uint32_t remoteAddr = (uint32_t)inet_addr(remoteIP);	

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = remoteAddr;
	addr.sin_port = htons(port);

	int32_t ret = ::connect(m_socket, (const struct sockaddr*)&addr, sizeof(addr));
	if (ret != 0)
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
			LogError("Socket Error: Connect Busy\n");
			m_state = SK_CONNECTING;
			return S_OK;
		}
		else
		{
			LogError("Socket Error: Connect Error %d\n", errorret);
			CloseSocket();
			return S_ERROR;			
		}
	}
	m_state = SK_CONNECTING;
	return S_OK;
}

void MSocket::CloseSocket()
{
	MSocketIO::CloseSocket();
	Clear();
}

void MSocket::Update()
{
	if (m_state == SK_SUCCESS)
		MSocketIO::Update();
	else if (m_state == SK_CONNECTING)	
		CheckConnect();
}

void MSocket::CheckConnect()
{
	int32_t ret = select_fd(m_socket, 0.01, WAIT_FOR_WRITE);
	if (ret > 0)
	{
		m_state = SK_SUCCESS;
		OnConnectSucc();
	}	
	else if (ret < 0)
	{
		m_state = SK_UNKNOWN;
		OnSocketError();
	}
}

void MSocket::OnConnectSucc()
{
}
