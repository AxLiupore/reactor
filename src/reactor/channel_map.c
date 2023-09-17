#include "../../include/reactor/channel_map.h"

struct channel_map* init_channel_map(int size)
{
	struct channel_map* map = (struct channel_map*)malloc(sizeof(struct channel_map));
	map->size = size;
	map->list = (struct channel**)malloc(sizeof(struct channel) * size);
	return map;
}

void clear_channel_map(struct channel_map* map)
{
	if (map != NULL)
	{
		for (int i = 0; i < map->size; i++)
		{
			if (map->list[i] != NULL)
			{
				free(map->list[i]); // 数组里面的元素全部被释放
			}
		}
		free(map->list); // 释放数组指向的地址
		map->list = NULL;
	}
	map->size = 0;
}

bool resize_memory_channel_map(struct channel_map* map, int new_size, int unit_size)
{
	if (map->size < new_size)
	{
		int cur_size = map->size;
		while (cur_size < new_size)
		{
			cur_size *= 2;
		}
		struct channel** temp = realloc(map->list, unit_size * cur_size);
		if (temp == NULL)
		{
			return false;
		}
		map->list = temp;
		memset(map->list[map->size], 0, (cur_size - map->size) * unit_size);
		map->size = cur_size;
	}
	return true;
}
