#include "../../include/server/tcp_server.h"

struct tcp_server* init_tcp_server(unsigned short port, int threadNum)
{
	struct tcp_server* tcpServer = (struct tcp_server*)malloc(sizeof(struct tcp_server));
	tcpServer->listener = init_listener(port);
	tcpServer->main_loop = init_event_loop(); // 主线程的eventLoop
	tcpServer->thread_num = threadNum;
	tcpServer->thread_pool = init_thread_pool(tcpServer->main_loop, threadNum);
	return tcpServer;
}

struct listener* init_listener(unsigned short port)
{
	struct listener* listener = (struct listener*)malloc(sizeof(struct listener));
	// 1.创建一个用于监听的文件描述符
	int lfd = socket(AF_INET, SOCK_STREAM, 0);
	if (lfd == -1)
	{
		perror("socket");
		return NULL;
	}
	// 2.设置端口复用
	int opt = 1;
	int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (ret == -1)
	{
		perror("setsockopt");
		return NULL;
	}
	// 3.绑定端口
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
	if (ret == -1)
	{
		perror("bind");
		return NULL;
	}
	// 4.设置监听
	ret = listen(lfd, 128);
	if (ret == -1)
	{
		perror("listen");
		return NULL;
	}
	listener->lfd = lfd;
	listener->port = port;
	return listener;
}
