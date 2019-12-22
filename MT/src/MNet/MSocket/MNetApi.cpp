#include "MNetDef.h"
#ifndef _WIN32

#include <fcntl.h>
#include <sys/ioctl.h>

#include <sys/param.h>
#include <sys/cdefs.h>
#include <sys/time.h>

#include "MNetApi.h"


int32_t set_non_block(int32_t fd)
{

	int flag = 0;
	flag = fcntl(fd, F_GETFL, 0);
	if (flag < 0)
	{
		return -1;
	}

	flag |= O_NONBLOCK;
	flag |= O_NDELAY;

	if (fcntl(fd, F_SETFL, flag) < 0)
	{
		return -1;
	}

	return 0;
}

char *inet_ntoa_f(uint32_t ip)
{
	static char buf[16];
	uint8_t *str = (uint8_t *)&ip;
	sprintf(buf, "%d.%d.%d.%d", str[0] & 0xff, str[1] & 0xff, str[2] & 0xff, str[3] & 0xff);

	return buf;
}

void Delay(uint32_t usec)
{
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);

	timeout.tv_sec = usec / 1000000;
	timeout.tv_usec = usec % 1000000;
	int ret = select(0, NULL, NULL, &fds, &timeout);
	if (0 > ret)
	{
		perror("select");
	}
}

#else

#include "MNetApi.h"

int32_t set_non_block(int32_t fd)
{
	u_long nNonBlocking = 1;
	ioctlsocket(fd, FIONBIO, &nNonBlocking);
	return 0;
}

char *inet_ntoa_f(uint32_t ip)
{
	static char buf[16];
	uint8_t *str = (uint8_t *)&ip;
	sprintf(buf, "%d.%d.%d.%d", str[0] & 0xff, str[1] & 0xff, str[2] & 0xff, str[3] & 0xff);

	return buf;
}

int epoll_create(int __size)
{
	return -1;
}

int epoll_ctl(int __epfd, int __op, int __fd, struct epoll_event *__ev)
{
	return -1;
}


int epoll_wait(int __epfd, struct epoll_event *__events, int __maxevents, int __timeout)
{
	return -1;
}


void Delay(uint32_t usec)
{
	struct timeval timeout;
	fd_set fds;
	FD_ZERO(&fds);

	timeout.tv_sec = usec / 1000000;
	timeout.tv_usec = usec % 1000000;
	int ret = select(0, NULL, NULL, &fds, &timeout);
	if (0 > ret)
	{
		perror("select");
	}
}

#endif

int32_t select_fd(int32_t fd, double maxtime, int32_t wait_for)
{
	fd_set fdset;
	fd_set *rd = NULL, *wr = NULL;
	struct timeval tmout;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);
	if (wait_for & WAIT_FOR_READ)
		rd = &fdset;
	if (wait_for & WAIT_FOR_WRITE)
		wr = &fdset;

	tmout.tv_sec = (long)maxtime;
	tmout.tv_usec = (long)(1000000 * (maxtime - (long)maxtime));
	
	return select(fd + 1, rd, wr, NULL, &tmout);
}

