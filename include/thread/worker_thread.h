#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

#include <pthread.h>
#include "../reactor/event_loop.h"

// 定义子线程对应的结构体
struct worker_thread
{
	pthread_t id;
	char name[32];
	pthread_mutex_t mutex; // 互斥锁
	pthread_cond_t cond; // 条件变量
	struct event_loop* event_loop; // 反应堆模型
};

// 初始化worker_thread
int init_worker_thread(struct worker_thread* thread, int index);

// 启动线程
void run_worker_thread(struct worker_thread* thread);

#endif
