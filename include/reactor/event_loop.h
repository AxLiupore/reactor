#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include "dispatcher.h"

extern struct dispatcher epoll_dispatcher; // 使用其他源文件里面的全局变量

struct event_loop
{
	struct dispatcher* dispatcher;
	void* dispatcher_data;
};

#endif
