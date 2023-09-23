#include "../../include/server/tcp_connection.h"

struct tcp_connection* init_tcp_connection(int fd, struct event_loop* eventLoop)
{
	struct tcp_connection* conn = (struct tcp_connection*)malloc(sizeof(struct tcp_connection));
	conn->event_loop = eventLoop;
	conn->read_buffer = init_buffer(10240);
	conn->write_buffer = init_buffer(10240);
	sprintf(conn->name, "Connection-%d", fd);
	conn->channel = init_channel(fd, READ_EVENT, , NULL, NULL);
	// 添加到事件循环中去
	add_task_event_loop(eventLoop, conn->channel, ADD);
	return conn;
}
