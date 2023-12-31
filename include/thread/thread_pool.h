#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "../reactor/event_loop.h"
#include "worker_thread.h"

// 定义线程池
struct thread_pool
{
	struct event_loop* main_loop; // 主线程的反应堆模型
	bool state;
	int thread_num;
	struct worker_thread* worker_threads;
	int index;
};

// 初始化线程池
struct thread_pool* init_thread_pool(struct event_loop* mainLoop, int count);

// 启动线程池
void run_thread_pool(struct thread_pool* pool);

// 取出线程池中的某个子线程的反应堆实例
struct event_loop* take_worker_event_loop(struct thread_pool* pool);

#endif
