#pragma once
#include <stdint.h>
#include <stdio.h>

#ifndef _WIN32

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
#define INVALID_SOCKET -1
#define closesocket close

//block flag
enum enmSocketBlockFlag
{
	enmSocketBlockFlag_lock = 0,
	enmSocketBlockFlag_nonblock = 1,
};

#ifdef __cplusplus
extern "C"
{
#endif

	int32_t set_non_block(int32_t fd);

	char *inet_ntoa_f(uint32_t ip);

	void Delay(uint32_t usec);

#ifdef __cplusplus
}
#endif



#else

/**
define epoll data on windows platform
*/
#include "MNetDef.h"

int32_t set_non_block(int32_t fd);

enum EPOLL_EVENTS
{
	EPOLLIN = 0x001,
#define EPOLLIN EPOLLIN
	EPOLLPRI = 0x002,
#define EPOLLPRI EPOLLPRI
	EPOLLOUT = 0x004,
#define EPOLLOUT EPOLLOUT
	EPOLLRDNORM = 0x040,
#define EPOLLRDNORM EPOLLRDNORM
	EPOLLRDBAND = 0x080,
#define EPOLLRDBAND EPOLLRDBAND
	EPOLLWRNORM = 0x100,
#define EPOLLWRNORM EPOLLWRNORM
	EPOLLWRBAND = 0x200,
#define EPOLLWRBAND EPOLLWRBAND
	EPOLLMSG = 0x400,
#define EPOLLMSG EPOLLMSG
	EPOLLERR = 0x008,
#define EPOLLERR EPOLLERR
	EPOLLHUP = 0x010,
#define EPOLLHUP EPOLLHUP
	EPOLLONESHOT = (1 << 30),
#define EPOLLONESHOT EPOLLONESHOT
	EPOLLET = (1 << 31)
#define EPOLLET EPOLLET
};

typedef union epoll_data
{
	void *ptr;
	int fd;
	uint32_t u32;
	uint64_t u64;
} epoll_data_t;

struct epoll_event
{
	uint32_t events;      /* Epoll events */
	epoll_data_t data;    /* User data variable */
};

#ifndef EPOLL_CTL_ADD
#define EPOLL_CTL_ADD 1 /* Add a file decriptor to the interface.  */
#endif

#ifndef EPOLL_CTL_DEL
#define EPOLL_CTL_DEL 2 /* Remove a file decriptor from the interface.  */
#endif

#ifndef EPOLL_CTL_MOD
#define EPOLL_CTL_MOD 3 /* Change file decriptor epoll_event structure.  */
#endif

void Delay(uint32_t usec);

int epoll_create(int __size);

int epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event * __ev);

int epoll_wait(int __epfd, struct epoll_event * __events, int __maxevents, int __timeout);

#endif

int32_t select_fd(int32_t fd, double maxtime, int32_t wait_for);
