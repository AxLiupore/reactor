#include "../../include/reactor/channel.h"

struct Channel* init_channel(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg)
{
	struct Channel* channel = (struct Channel*)malloc(sizeof(struct Channel));
	channel->fd = fd;
	channel->events = events;
	channel->readCallBack = readFunc;
	channel->writeCallBack = writeFunc;
	channel->arg = arg;
	return channel;
}

void write_event_enable(struct Channel* channel, bool flag)
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

bool check_write_event_enable(struct Channel* channel)
{
	return channel->events & WRITE_EVENT;
}
