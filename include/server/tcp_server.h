#ifndef _TCP_SERVER_H_
#define _TCP_SERVER_H_

#include <arpa/inet.h>
#include "../reactor/event_loop.h"
#include "../thread/thread_pool.h"

struct listener
{
	int lfd;
	unsigned short port;
};

struct tcp_server
{
	int thread_num; // 线程池的数量
	struct event_loop* main_loop;
	struct thread_pool* thread_pool;
	struct listener* listener;
};

// 初始化
struct tcp_server* init_tcp_server(unsigned short port, int threadNum);

// 初始化监听
struct listener* init_listener(unsigned short port);

// 启动服务器
void run_tcp_server(struct tcp_server* tcpServer);

#endif
