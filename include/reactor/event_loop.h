#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include "dispatcher.h"

struct event_loop
{
	struct dispatcher* dispatcher;
	void* dispatcher_data;
};

#endif
