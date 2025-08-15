#include "medium.h"

static int get_free_slot(size_t *slot, size_t size)
{
	size_t i;

	i = 0;
	pthread_mutex_lock(&g_malloc.lock);
	while (i < size)
	{
		if (slot[i] == 0)
		{
			slot[i] = 1;
			pthread_mutex_unlock(&g_malloc.lock);
			return (i);
		}
		i++;
	}
	pthread_mutex_unlock(&g_malloc.lock);
	return (-1);
}

size_t get_slot_id(const char *ptr)
{
	if (!ptr)
		print_panic("get_slot_id");
	const char *sbase = (const char *)g_malloc.reserved_memory.small;
	const size_t sbytes = g_malloc.reserved_memory.small_byte_size;
	const char *mbase = (const char *)g_malloc.reserved_memory.medium;
	const size_t mbytes = g_malloc.reserved_memory.medium_byte_size;
	if (ptr >= sbase && ptr < sbase + sbytes)
		return ((size_t)(ptr - sbase) / SMALL_MALLOC);
	if (ptr >= mbase && ptr < mbase + mbytes)
		return ((size_t)(ptr - mbase) / MEDIUM_MALLOC);
	print_panic("get_slot_id");
	return 0;
}

void *format_from_pool(size_t size, int type)
{
	int id;
	size_t slotsSize;
	void *memoryPool;

	if (type == SMALL)
	{
		slotsSize = SMALL_MALLOC;
		memoryPool = g_malloc.reserved_memory.small;
		id = get_free_slot(g_malloc.reserved_memory.free_small, g_malloc.reserved_memory.small_slot_size);
		g_malloc.reserved_memory.free_small[id] = size;
	}
	else
	{
		slotsSize = MEDIUM_MALLOC;
		memoryPool = g_malloc.reserved_memory.medium;
		id = get_free_slot(g_malloc.reserved_memory.free_medium, g_malloc.reserved_memory.medium_slot_size);
		g_malloc.reserved_memory.free_medium[id] = size;
	}
	if (id == -1)
		return NULL;
	void *block = (char *)memoryPool + (id * slotsSize);
	return block;
}

int which_pool(const char *ptr)
{
	if (!ptr)
		print_panic("which pool");
	const char *sbase = (const char *)g_malloc.reserved_memory.small;
	const size_t sbytes = g_malloc.reserved_memory.small_byte_size;
	const char *mbase = (const char *)g_malloc.reserved_memory.medium;
	const size_t mbytes = g_malloc.reserved_memory.medium_byte_size;

	if (ptr >= sbase && ptr < sbase + sbytes)
	{
		if (((size_t)(ptr - sbase) % SMALL_MALLOC) == 0)
			return SMALL;
		// print_panic("Memory not aligned: (ptr - sbase) % SMALL_MALLOC != 0");
	}
	if (ptr >= mbase && ptr < mbase + mbytes)
	{
		if (((size_t)(ptr - mbase) % MEDIUM_MALLOC) == 0)
			return MEDIUM;
		// print_panic("Memory not aligned: (ptr - mbase) % MEDIUM_MALLOC != 0");
	}
	return LARGE;
}