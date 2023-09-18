#include "../../include/reactor/event_loop.h"

#define INIT_CHANNEL_MAP_SIZE 128

// 主线程
struct event_loop* init_event_loop()
{
	return init_self_event_loop(NULL);
}

// 子线程
struct event_loop* init_self_event_loop(const char* threadName)
{
	struct event_loop* eventLoop = (struct event_loop*)malloc(sizeof(struct event_loop));
	eventLoop->state = false;
	eventLoop->thread_id = pthread_self();
	pthread_mutex_init(&eventLoop->mutex, NULL);
	strcpy(eventLoop->thread_name, threadName == NULL ? "main_thread" : threadName); // 内存的拷贝
	eventLoop->dispatcher = &epoll_dispatcher;
	eventLoop->dispatcher_data = eventLoop->dispatcher->init();
	eventLoop->head = NULL;
	eventLoop->tail = NULL;
	eventLoop->channel_map = init_channel_map(INIT_CHANNEL_MAP_SIZE);
	return eventLoop;
}

// 启动反应堆模型
int start_event_loop(struct event_loop* eventLoop)
{
	assert(eventLoop != NULL);
	// 取出事件分发器和检测模型
	struct dispatcher* dispatcher = eventLoop->dispatcher;
	//比较线程ID是否正常
	if (eventLoop->thread_id != pthread_self())
	{
		return -1;
	}
	// 循环进行事件处理
	while (!eventLoop->state)
	{
		dispatcher->dispatch(eventLoop, 2); // 超时2s
	}
	return 0;
}