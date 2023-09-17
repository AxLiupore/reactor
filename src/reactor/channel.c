#include "../../include/reactor/channel.h"

struct channel* init_channel(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
	struct channel* channel = (struct channel*)malloc(sizeof(struct channel));
	channel->fd = fd;
	channel->events = events;
	channel->readCallBack = readFunc;
	channel->writeCallBack = writeFunc;
	channel->arg = arg;
	return channel;
}

void write_event_enable(struct channel* channel, bool flag)
{
	if (flag)
	{
		channel->events |= WRITE_EVENT;
	}
	else
	{
		channel->events &= ~WRITE_EVENT;
	}
}

bool check_write_event_enable(struct channel* channel)
{
	return channel->events & WRITE_EVENT;
}
