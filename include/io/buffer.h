#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdlib.h>
#include <string.h>

struct buffer
{
	char* data; // 指向内存的指针
	int capacity; // buffer内存块的总大小，总的字节数
	int read_pos;
	int write_pos;
};

// 初始化
struct buffer* init_buffer(int size);

// 销毁内存的函数
void destroy_buffer(struct buffer* buffer);

// 扩容
void extend_room_buffer(struct buffer* buffer, int size);

// 获取剩余的可写内存容量
int write_enable_size_buffer(struct buffer* buffer);

// 获取剩余的可读内存容量
int read_enable_size_buffer(struct buffer* buffer);

#endif
