#include <sys/select.h>
#include "../../include/reactor/dispatcher.h"

#define SELECT_MAX_EVENTS 1024

struct data_select
{
	fd_set read_set;
	fd_set write_set;

};

static void* init_select();

static int add_select(struct channel* channel, struct event_loop* eventLoop);

static int remove_select(struct channel* channel, struct event_loop* eventLoop);

static int modify_select(struct channel* channel, struct event_loop* eventLoop);

static int dispatch_select(struct event_loop* eventLoop, int timeout);

static int clear_select(struct event_loop* eventLoop);

static void set_fd_set(struct channel* channel, struct data_select* data);

static void clear_fd_set(struct channel* channel, struct data_select* data);

struct dispatcher select_dispatcher =
	{
		init_select,
		add_select,
		remove_select,
		modify_select,
		dispatch_select,
		clear_select
	};

void* init_select()
{
	struct data_select* data = (struct data_select*)malloc(sizeof(struct data_select));
	// 给集合清空，将所有的标志为设置为0
	FD_ZERO(&data->read_set);
	FD_ZERO(&data->write_set);
	return data;
}

void set_fd_set(struct channel* channel, struct data_select* data)
{
	if (channel->events & READ_EVENT)
	{
		FD_SET(channel->fd, &data->read_set); // 将文件描述符设置到读事件当中
	}
	if (channel->events & WRITE_EVENT)
	{
		FD_SET(channel->fd, &data->write_set);
	}
}

void clear_fd_set(struct channel* channel, struct data_select* data)
{
	if (channel->events & READ_EVENT)
	{
		FD_CLR(channel->fd, &data->read_set); // 将文件描述符设置到读事件当中
	}
	if (channel->events & WRITE_EVENT)
	{
		FD_CLR(channel->fd, &data->write_set);
	}
}

int add_select(struct channel* channel, struct event_loop* eventLoop)
{
	struct data_select* data = (struct data_select*)eventLoop->dispatcher_data;
	if (channel->fd >= SELECT_MAX_EVENTS)
	{
		return -1;
	}
	set_fd_set(channel, data);
	return 0;
}

int remove_select(struct channel* channel, struct event_loop* eventLoop)
{
	struct data_select* data = (struct data_select*)eventLoop->dispatcher_data;
	clear_fd_set(channel, data);
	return 0;
}

int modify_select(struct channel* channel, struct event_loop* eventLoop)
{
	struct data_select* data = (struct data_select*)eventLoop->dispatcher_data;
	clear_fd_set(channel, data);
	set_fd_set(channel, data);
	return 0;
}

int dispatch_select(struct event_loop* eventLoop, int timeout)
{
	struct data_select* data = (struct data_select*)eventLoop->dispatcher_data;
	struct timeval time_val;
	time_val.tv_sec = timeout;
	time_val.tv_usec = 0;
	// 对原始数据进行备份
	fd_set read_temp = data->read_set;
	fd_set write_temp = data->write_set;
	int count = select(SELECT_MAX_EVENTS, &read_temp, &write_temp, NULL, &time_val);
	if (count == -1)
	{
		perror("select error");
		exit(0);
	}
	else if (count == 0)
	{
		perror("select error");
		return -1;
	}
	else
	{
		for (int i = 0; i < SELECT_MAX_EVENTS; i++)
		{
			// 被激活了，去读取数据
			if (FD_ISSET(i, &read_temp))
			{
				active_event(eventLoop, i, READ_EVENT); // i文件描述符发生了读事件
			}
			if (FD_ISSET(i, &write_temp))
			{
				active_event(eventLoop, i, WRITE_EVENT);
			}
		}
	}
	return 0;
}

int clear_select(struct event_loop* eventLoop)
{
	struct data_select* data = (struct data_select*)eventLoop->dispatcher_data;
	free(data);
	return 0;
}