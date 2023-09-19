#include "../../include/io/buffer.h"

// 初始化
struct buffer* init_buffer(int size)
{
	struct buffer* buffer = (struct buffer*)malloc(sizeof(struct buffer));
	if (buffer->data != NULL)
	{
		buffer->data = (char*)malloc(size);
		buffer->capacity = size;
		buffer->read_pos = buffer->write_pos = 0;
		memset(buffer->data, 0, size);
	}
	return buffer;
}

// 销毁内存的函数
void destroy_buffer(struct buffer* buffer)
{
	if (buffer != NULL)
	{
		if (buffer->data != NULL)
		{
			free(buffer->data);
		}
	}
	free(buffer);
}