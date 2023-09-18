#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include <pthread.h>
#include "dispatcher.h"
#include "channel_map.h"

// 使用其他源文件里面的全局变量
extern struct dispatcher epoll_dispatcher;
extern struct dispatcher select_dispatcher;
extern struct dispatcher poll_dispatcher;

// 处理该节点的channel的方式
enum ELEMENT_TYPE
{
	ADD,
	DELETE,
	MODIFY
};

// 定义任务队列的节点
struct channel_element
{
	int type; // 如何 处理该节点中的channel
	struct channel* channel;
	struct channel_element* next;
};

struct event_loop
{
	struct dispatcher* dispatcher; // 就是三个其中的一个
	void* dispatcher_data; // 记录dispatcher的信息
	bool state; // 表示是否退出
	struct channel_element* head; // 任务队列头节点
	struct channel_element* tail; // 任务队列尾节点
	struct channel_map* channel_map; // map
	pthread_t thread_id; // 属于的那个线程的id
	char thread_name[32];
	pthread_mutex_t mutex; // 这个event_loop的互斥锁
};

// 初始化：自线程
struct event_loop* init_self_event_loop(const char* threadName);

// 初始化：主线程
struct event_loop* init_event_loop();

#endif
