#ifndef _TCP_CONNECTION_H_
#define _TCP_CONNECTION_H_

#include "../reactor/event_loop.h"
#include "../io/buffer.h"
#include "../reactor/channel.h"

struct tcp_connection
{
	struct event_loop* event_loop;
	struct channel* channel;
	struct buffer* read_buffer; // 接受套接字数据的
	struct buffer* write_buffer; // 发送套接字数据的
	char name[32];
};

// 初始化
struct tcp_connection* init_tcp_connection(int fd, struct event_loop* eventLoop);

#endif