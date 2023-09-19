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

// 启动线程池
void run_thread_pool(struct thread_pool* pool)
{
	assert(pool && !pool->state);
	if (pool->main_loop->thread_id != pthread_self())
	{
		exit(0);
	}
	pool->state = true;
	if (pool->thread_num > 0)
	{
		// 初始化子线程
		for (int i = 0; i < pool->thread_num; i++)
		{
			init_worker_thread(&pool->worker_threads[i], i);
			run_worker_thread(&pool->worker_threads[i]);
		}
	}
}

// 取出线程池中的某个子线程的反应堆实例
struct event_loop* take_worker_event_loop(struct thread_pool* pool)
{
	assert(pool->state);
	if (pool->main_loop->thread_id != pthread_self())
	{
		exit(0);
	}
	// 从线程池中找到一个子线程，然后取出里面的反应堆实例
	struct event_loop* eventLoop = pool->main_loop;
	if (pool->thread_num > 0)
	{
		eventLoop = pool->worker_threads[pool->index].event_loop;
		pool->index = (pool->index + 1) % pool->thread_num;
	}
	return eventLoop;
}