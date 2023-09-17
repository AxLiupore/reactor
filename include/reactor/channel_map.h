#ifndef REACTOR_CHANNEL_MAP_H
#define REACTOR_CHANNEL_MAP_H

#include "channel.h"

struct channel_map
{
	int size; // 记录指针指向的数组的元素总个数
	struct channel** list; // 存储的是channel*[]
};

// 初始化一个channel_map
struct channel_map* init_channel_map(int size);

// 清空map
void clear_channel_map(struct channel_map* map);

// 重新分配内存空间
bool resize_memory_channel_map(struct channel_map* map, int new_size, int unit_size);

#endif
