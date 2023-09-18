#include <sys/epoll.h>
#include "../../include/reactor/dispatcher.h"

#define EPOLL_MAX_EVENTS 1024

// 这个就是一个epoll树
struct data_epoll
{
	int fd_epoll;
	struct epoll_event* events;
};

static void* init_epoll();

static int add_epoll(struct channel* channel, struct event_loop* eventLoop);

static int remove_epoll(struct channel* channel, struct event_loop* eventLoop);

static int modify_epoll(struct channel* channel, struct event_loop* eventLoop);

static int dispatch_epoll(struct event_loop* eventLoop, int timeout);

static int clear_epoll(struct event_loop* eventLoop);

// 对epoll树上的文件描述符进行控制：add, remove, modify
static int ctl_epoll(struct channel* channel, struct event_loop* eventLoop, int op);

// 结构体内容是函数指针，将函数依次指定给它
struct dispatcher epoll_dispatcher =
	{
		init_epoll,
		add_epoll,
		remove_epoll,
		modify_epoll,
		dispatch_epoll,
		clear_epoll
	};

static void* init_epoll()
{
	struct data_epoll* data = (struct data_epoll*)malloc(sizeof(struct data_epoll));
	data->fd_epoll = epoll_create(10); // 同时监视的文件描述符
	if (data->fd_epoll == -1)
	{
		perror("epoll_create");
		exit(0);
	}
	data->events = (struct epoll_event*)calloc(EPOLL_MAX_EVENTS, sizeof(struct epoll_event));
	return data;
}

int ctl_epoll(struct channel* channel, struct event_loop* eventLoop, int op)
{
	struct data_epoll* data = (struct data_epoll*)eventLoop->dispatcher_data;
	struct epoll_event event; // 事件
	event.data.fd = channel->fd;
	int events = 0;
	// 如果channel里面的事件有这个事件，就将这个事件记录到events里面
	if (channel->events & READ_EVENT)
	{
		events |= EPOLLIN;
	}
	if (channel->events & WRITE_EVENT)
	{
		events |= EPOLLOUT;
	}
	event.data.fd = channel->fd;
	event.events = events;
	// 将channel-fd的文件描述符添加到data->fd_epoll树上，检测event事件
	int ret = epoll_ctl(data->fd_epoll, op, channel->fd, &event);
	return ret; // 0:success, -1:failed
}

static int add_epoll(struct channel* channel, struct event_loop* eventLoop)
{
	int ret = ctl_epoll(channel, eventLoop, EPOLL_CTL_ADD);
	if (ret == -1)
	{
		perror("epoll_ctl add");
		exit(0);
	}
	return ret;
}

static int remove_epoll(struct channel* channel, struct event_loop* eventLoop)
{
	int ret = ctl_epoll(channel, eventLoop, EPOLL_CTL_DEL);
	if (ret == -1)
	{
		perror("epoll_ctl remove");
		exit(0);
	}
	return ret;
}

static int modify_epoll(struct channel* channel, struct event_loop* eventLoop)
{
	int ret = ctl_epoll(channel, eventLoop, EPOLL_CTL_MOD);
	if (ret == -1)
	{
		perror("epoll_ctl modify");
		exit(0);
	}
	return ret;
}

static int dispatch_epoll(struct event_loop* eventLoop, int timeout)
{
	struct data_epoll* data = (struct data_epoll*)eventLoop->dispatcher_data;
	// count: -1 错误, 0 超时, >0 事件发生的个数
	int count = epoll_wait(data->fd_epoll, data->events, EPOLL_MAX_EVENTS, timeout * 1000);
	if (count == -1)
	{
		perror("epoll_wait error");
		exit(0);
	}
	else if (count == 0)
	{
		perror("epoll_wait timeout");
		return -1;
	}
	else
	{
		for (int i = 0; i < count; i++)
		{
			int events = data->events[i].events; // 触发的事件
			int fd = data->events[i].data.fd; // 对应的文件描述符
			// 出现错误：对端断开了连接，删除这个文件描述符
			if (events & EPOLLERR || events & EPOLLHUP)
			{
				epoll_ctl(data->fd_epoll, EPOLL_CTL_DEL, fd, NULL);
				continue;
			}
			if (events & EPOLLIN)
			{
				active_event(eventLoop, fd, READ_EVENT);
			}
			if (events & EPOLLOUT)
			{
				active_event(eventLoop, fd, WRITE_EVENT);
			}
		}
	}
	return 0;
}

static int clear_epoll(struct event_loop* eventLoop)
{
	struct data_epoll* data = (struct data_epoll*)eventLoop->dispatcher_data;
	free(data->events);
	close(data->fd_epoll);
	free(data);
	return 0;
}


