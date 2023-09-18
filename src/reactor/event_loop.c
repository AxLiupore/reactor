#include "../../include/reactor/event_loop.h"

#define INIT_CHANNEL_MAP_SIZE 128

// 主线程
struct event_loop* init_event_loop()
{
	return init_self_event_loop(NULL);
}

// 写数据
void write_local_message(struct event_loop* eventLoop)
{
	const char* message = "AxLiu love Xiaoyu forever";
	write(eventLoop->socketPair[0], message, strlen(message));
}

// 读数据：接受socketpair[0]发送过来的数据
int read_local_message(void* arg)
{
	struct event_loop* eventLoop = (struct event_loop*)arg;
	char buf[256];
	read(eventLoop->socketPair[1], buf, sizeof(buf));
	return 0;
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
	int ret = socketpair(AF_UNIX, SOCK_STREAM, 0, eventLoop->socketPair);
	if (ret == -1)
	{
		perror("socketpair");
		exit(0);
	}
	// 制定规则：eventLoop->socketpair[0]发送数据，eventLoop->socketpair[1]接受数据
	struct channel* channel = init_channel(eventLoop->socketPair[1], READ_EVENT, read_local_message, NULL, eventLoop);
	// channel添加到任务队列中
	add_task_event_loop(eventLoop, channel, ADD);
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
		process_task_event_loop(eventLoop);
	}
	return 0;
}

// 处理激活的文件描述符
int active_event(struct event_loop* eventLoop, int fd, int event)
{
	if (fd < 0 || eventLoop == NULL)
	{
		return -1;
	}
	// 当读事件或写事件被触发之后，通过fd找到channel
	struct channel* channel = eventLoop->channel_map->list[fd];
	assert(channel->fd == fd);
	// 执行回调函数
	if (event & READ_EVENT && channel->readCallBack != NULL)
	{
		channel->readCallBack(channel->arg);
	}
	if (event & WRITE_EVENT && channel->writeCallBack != NULL)
	{
		channel->writeCallBack(channel->arg);
	}
	return 0;
}

// 添加任务到任务队列
int add_task_event_loop(struct event_loop* eventLoop, struct channel* channel, enum ELEMENT_TYPE type)
{
	// 加锁，保护共享资源
	pthread_mutex_lock(&eventLoop->mutex);
	// 创建新节点
	struct channel_element* node = (struct channel_element*)malloc(sizeof(struct channel_element));
	node->channel = channel;
	node->type = type;
	node->next = NULL;
	// 链表为空
	if (eventLoop->head == NULL)
	{
		eventLoop->head = eventLoop->tail = node;
	}
	else
	{
		eventLoop->tail->next = node;
		eventLoop->tail = node;
	}
	pthread_mutex_unlock(&eventLoop->mutex);
	// 处理节点
	if (eventLoop->thread_id == pthread_self())
	{
		// 当前子线程
		process_task_event_loop(eventLoop);
	}
	else
	{
		// 主线程 -- 告诉子线程处理任务队列中的任务
		write_local_message(eventLoop);
	}
	return 0;
}

// 处理任务队列中的任务
int process_task_event_loop(struct event_loop* eventLoop)
{
	pthread_mutex_lock(&eventLoop->mutex);
	// 取出头节点
	struct channel_element* head = eventLoop->head;
	while (head != NULL)
	{
		struct channel* channel = head->channel;
		if (head->type == ADD)
		{

		}
		else if (head->type == DELETE)
		{

		}
		else if (head->type == MODIFY)
		{

		}
		struct channel_element* temp = head;
		head = head->next;
		free(temp);
	}
	eventLoop->head = eventLoop->tail = NULL;
	pthread_mutex_unlock(&eventLoop->mutex);
	return 0;
}