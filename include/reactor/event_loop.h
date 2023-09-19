#ifndef REACTOR_EVENT_LOOP_H
#define REACTOR_EVENT_LOOP_H

#include <pthread.h>
#include <assert.h>
#include <sys/socket.h>
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
	int socketPair[2]; // 存储本地通信的fd通过socketpair初始化
};

// 初始化：自线程
struct event_loop* init_self_event_loop(const char* threadName);

// 初始化：主线程
struct event_loop* init_event_loop();

// 启动反应堆模型
int run_event_loop(struct event_loop* eventLoop);

// 处理激活的文件描述符
int active_event(struct event_loop* eventLoop, int fd, int event);

//添加任务到任务队列
int add_task_event_loop(struct event_loop* eventLoop, struct channel* channel, enum ELEMENT_TYPE type);

// 处理任务队列中的任务
int process_task_event_loop(struct event_loop* eventLoop);

// 将任务队列中的task中的channel添加、修改、删除->dispatcher
int add_dispatcher_event_loop(struct event_loop* eventLoop, struct channel* channel);

int delete_dispatcher_event_loop(struct event_loop* eventLoop, struct channel* channel);

int modify_dispatcher_event_loop(struct event_loop* eventLoop, struct channel* channel);

// 释放channel
int destroy_channel(struct event_loop* eventLoop, struct channel* channel);

#endif
