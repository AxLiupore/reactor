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

// 用于与客户端进行通信
int accept_connection(void* arg)
{
	struct tcp_server* tcpServer = (struct tcp_server*)arg;
	// 和客户端进行连接
	int cfd = accept(tcpServer->listener->lfd, NULL, NULL);
	// 从线程池中取出一个子线程的反应堆实例，去处理这个cfd
	struct event_loop* eventLoop = take_worker_event_loop(tcpServer->thread_pool);
	// 将cfd放到tcp_connection中去处理
	init_tcp_connection(cfd, eventLoop);
}

void run_tcp_server(struct tcp_server* tcpServer)
{
	// 启动线程池
	run_thread_pool(tcpServer->thread_pool);
	// 添加检测的任务
	struct channel* channel = init_channel(tcpServer->listener->lfd, READ_EVENT, accept_connection, NULL, tcpServer);
	add_task_event_loop(tcpServer->main_loop, channel, ADD);
	// 启动反应堆模型
	run_event_loop(tcpServer->main_loop);
}
