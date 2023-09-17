#include <sys/poll.h>
#include "../../include/reactor/dispatcher.h"

#define POLL_MAX_EVENTS 1024

struct data_poll
{
	int max_fd;
	struct pollfd fds[POLL_MAX_EVENTS];
};

static void* init_poll();

static int add_poll(struct channel* channel, struct event_loop* eventLoop);

static int remove_poll(struct channel* channel, struct event_loop* eventLoop);

static int modify_poll(struct channel* channel, struct event_loop* eventLoop);

static int dispatch_poll(struct event_loop* eventLoop, int timeout);

static int clear_poll(struct event_loop* eventLoop);

struct dispatcher poll_dispatcher =
	{
		init_poll,
		add_poll,
		remove_poll,
		modify_poll,
		dispatch_poll,
		clear_poll
	};

void* init_poll()
{
	struct data_poll* data = (struct data_poll*)malloc(sizeof(struct data_poll));
	data->max_fd = 0;
	for (int i = 0; i < POLL_MAX_EVENTS; i++)
	{
		data->fds[i].fd = -1;
		data->fds[i].events = 0;
		data->fds[i].revents = 0;
	}
	return data;
}

int add_poll(struct channel* channel, struct event_loop* eventLoop)
{
	struct data_poll* data = (struct data_poll*)eventLoop->dispatcher_data;
	int events = 0;
	if (channel->events & READ_EVENT)
	{
		events |= POLLIN;
	}
	if (channel->events & WRITE_EVENT)
	{
		events |= POLLOUT;
	}
	// 找到空闲的没有被占用的元素，将channel->fd放到poll树上去检测
	int i;
	for (i = 0; i < POLL_MAX_EVENTS; i++)
	{
		if (data->fds[i].fd == -1)
		{
			data->fds[i].events = events;
			data->fds[i].fd = channel->fd;
			data->max_fd = data->max_fd < i ? i : data->max_fd;
			break;
		}
	}
	if (i >= POLL_MAX_EVENTS)
	{
		return -1;
	}
	return 0;
}

int remove_poll(struct channel* channel, struct event_loop* eventLoop)
{
	struct data_poll* data = (struct data_poll*)eventLoop->dispatcher_data;
	int i;
	for (i = 0; i < POLL_MAX_EVENTS; i++)
	{
		if (data->fds[i].fd == channel->fd)
		{
			data->fds[i].fd = -1;
			data->fds[i].events = 0;
			data->fds[i].revents = 0;
			break;
		}
	}
	if (i >= POLL_MAX_EVENTS)
	{
		return -1;
	}
	return 0;
}

int modify_poll(struct channel* channel, struct event_loop* eventLoop)
{
	struct data_poll* data = (struct data_poll*)eventLoop->dispatcher_data;
	int events = 0;
	if (channel->events & READ_EVENT)
	{
		events |= POLLIN;
	}
	if (channel->events & WRITE_EVENT)
	{
		events |= POLLOUT;
	}
	int i;
	for (i = 0; i < POLL_MAX_EVENTS; i++)
	{
		if (data->fds[i].fd == channel->fd)
		{
			data->fds[i].events = events;
			break;
		}
	}
	if (i >= POLL_MAX_EVENTS)
	{
		return -1;
	}
	return 0;
}

int dispatch_poll(struct event_loop* eventLoop, int timeout)
{
	struct data_poll* data = (struct data_poll*)eventLoop->dispatcher_data;
	int count = poll(data->fds, data->max_fd + 1, timeout * 1000); // 检测poll树上的事件
	if (count == -1)
	{
		perror("poll error");
		exit(0);
	}
	else if (count == 0)
	{
		perror("poll timeout");
		return -1;
	}
	else
	{
		for (int i = 0; i <= data->max_fd; i++)
		{
			if (data->fds[i].fd != -1)
			{
				if (data->fds[i].revents & POLLIN)
				{

				}
				if (data->fds[i].events & POLLOUT)
				{

				}
			}
		}
	}
	return 0;
}

int clear_poll(struct event_loop* eventLoop)
{
	struct data_poll* data = (struct data_poll*)eventLoop->dispatcher_data;
	free(data);
	return 0;
}