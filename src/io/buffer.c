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

// 扩容
void extend_room_buffer(struct buffer* buffer, int size)
{
	// 1.内存够用 -- 不需要扩容
	if (write_enable_size_buffer(buffer) >= size)
	{
		return;
	}
		// 2.内存需要合并才够用 -- 不需要扩容（剩余的可写的内存 + 已读的内存 >= size	）
	else if (buffer->read_pos + write_enable_size_buffer(buffer) >= size)
	{
		// 得到未读的内存的大小
		int readable = read_enable_size_buffer(buffer);
		// 移动内存
		memcpy(buffer->data, buffer->data + buffer->read_pos, readable);
		// 更新位置
		buffer->read_pos = 0;
		buffer->write_pos = readable;
	}
		// 3.内存不够用 -- 需要扩容
	else
	{
		void* temp = realloc(buffer->data, buffer->capacity + size);
		if (temp == NULL)
		{
			return; // 失败了
		}
		memset(temp + buffer->capacity, 0, size);
		// 更新数据
		buffer->data = temp;
		buffer->capacity += size;
	}
}

int write_enable_size_buffer(struct buffer* buffer)
{
	return buffer->capacity - buffer->write_pos;
}

int read_enable_size_buffer(struct buffer* buffer)
{
	return buffer->write_pos - buffer->read_pos;
}
