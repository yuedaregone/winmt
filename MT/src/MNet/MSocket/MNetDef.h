#pragma once

#ifdef _WIN32
#include <stdio.h>
#include <stdint.h>
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifdef _WIN32
typedef SOCKET	SocketFD;
#else
typedef int32_t SocketFD;
#endif

#define IP_MAX_LEN 20

#define LogError printf

#ifndef S_OK
#define S_OK 0
#endif
#define S_ERROR -1

#ifdef _WIN32
#define GET_ERROR_CODE WSAGetLastError()
#else
#define GET_ERROR_CODE errno
#endif

#define BUFFER_SIZE					1024*8
#define BUFFER_SOCKET_DATA_SIZE		0x00040000

typedef enum
{
	SocketUnknown,
	SocketListen,
	SocketCommunication,	
}SocketType;

enum {
	WAIT_FOR_READ = 1,
	WAIT_FOR_WRITE = 2
};

enum
{
	InvalideSocket = -1,
};

typedef enum
{
	SK_NOACTIVATE = -1,

	SK_SUCCESS,

	SK_CREATED,
	
	SK_CONNECTING,	
	
	SK_DISCONNECT,
	
	SK_RECVDATA_ERROR,

	SK_UNKNOWN,
}SocketError;


#pragma pack(1)

typedef struct  
{
	uint32_t tag;
	uint16_t length;
}MSG_HEAD;

#define MSG_TAG 0xFEEFFEEF

#pragma pack()
