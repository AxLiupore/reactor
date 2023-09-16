#ifndef REACTOR_CHANNEL_H
#define REACTOR_CHANNEL_H

#include <string.h>
#include <malloc.h>
#include <stdbool.h>

// 定义函数指针
typedef int (* handleFunc)(void* arg);

// 定义文件描述符的读写事件
enum FD_EVENT
{
	TIME_OUT = 0x01,
	READ_EVENT = 0x02,
	WRITE_EVENT = 0x04
};

struct Channel
{
	int fd; // 文件描述符
	int events; // 事件：r、w、rw
	// 事件触发之后的回调函数：w、r 回调
	handleFunc readCallBack;
	handleFunc writeCallBack;
	void* arg; // 回调函数的参数
};

// 初始化一个Channel
struct Channel* init_channel(int fd, int events, handleFunc readFunc, handleFunc writeFunc, void* arg);

// 修改fd的写事件（检测or不检测）
void write_event_enable(struct Channel* channel, bool flag);

// 判断是否需要检测文件描述符的写事件
bool check_write_event_enable(struct Channel* channel);

#endif