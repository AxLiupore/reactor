#ifndef REACTOR_DISPATCHER_H
#define REACTOR_DISPATCHER_H

#include <stdlib.h>
#include <unistd.h>
#include "channel.h"
#include "event_loop.h"

struct dispatcher
{
	// init -- 初始化epoll、poll或select需要的数据块
	void* (* init)();

	// add
	int (* add)(struct channel* channel, struct event_loop* eventLoop);

	// remove
	int (* remove)(struct channel* channel, struct event_loop* eventLoop);

	// modify
	int (* modify)(struct channel* channel, struct event_loop* eventLoop);

	// dispatch
	int (* dispatch)(struct event_loop* eventLoop, int timeout); // 单位：s

	// clear
	int (* clear)(struct event_loop* eventLoop);
};

#endif
