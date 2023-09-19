#include "../../include/thread/worker_thread.h"

// 初始化worker_thread
int init_worker_thread(struct worker_thread* thread, int index)
{
	thread->event_loop = NULL;
	thread->id = 0;
	sprintf(thread->name, "Sub-Thread-%d", index);
	pthread_mutex_init(&thread->mutex, NULL);
	pthread_cond_init(&thread->cond, NULL);
	return 0;
}

// 子线程的回调函数
static void* sub_running_thread(void* arg)
{
	struct worker_thread* thread = (struct worker_thread*)arg;
	pthread_mutex_lock(&thread->mutex);
	thread->event_loop = init_self_event_loop(thread->name);
	pthread_mutex_unlock(&thread->mutex);
	pthread_cond_signal(&thread->cond);
	run_event_loop(thread->event_loop);
	return NULL;
}

// 启动线程
void run_worker_thread(struct worker_thread* thread)
{
	// 创建子线程
	pthread_create(&thread->id, NULL, sub_running_thread, thread);
	// 阻塞主线程，让当前函数不会直接结束
	pthread_mutex_lock(&thread->mutex);
	while (thread->event_loop == NULL)
	{
		pthread_cond_wait(&thread->cond, &thread->mutex);
	}
	pthread_mutex_unlock(&thread->mutex);
}