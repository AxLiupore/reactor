#include "../../include/thread/thread_pool.h"

// 初始化线程池
struct thread_pool* init_thread_pool(struct event_loop* mainLoop, int count)
{
	struct thread_pool* pool = (struct thread_pool*)malloc(sizeof(struct thread_pool));
	pool->index = 0;
	pool->state = false;
	pool->main_loop = mainLoop;
	pool->worker_threads = (struct worker_thread*)malloc(sizeof(struct worker_thread) * count);
	return pool;
}