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
	size_t i = 0;
	while (i < LARGE_ALLOC_COUNT)
	{
		if (g_malloc.reserved_memory.large[i] == (void *)ptr)
			return (i);
		i++;
	}
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
		if (id == -1)
			return (NULL);
		g_malloc.reserved_memory.free_small[id] = size;
	}
	else if (type == MEDIUM)
	{
		slotsSize = MEDIUM_MALLOC;
		memoryPool = g_malloc.reserved_memory.medium;
		id = get_free_slot(g_malloc.reserved_memory.free_medium, g_malloc.reserved_memory.medium_slot_size);
		if (id == -1)
			return (NULL);
		g_malloc.reserved_memory.free_medium[id] = size;
	}
	else
	{
		size_t mlen = page_round_up(size);
		void *node = mmap(NULL, mlen, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		id = get_free_slot(g_malloc.reserved_memory.free_large, g_malloc.reserved_memory.large_slot_size);
		if (id == -1)
			return (NULL);
		g_malloc.reserved_memory.free_large[id] = size;
		g_malloc.reserved_memory.large[id] = node;
		return node;
	}
	return (char *)memoryPool + (id * slotsSize);
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
	}
	if (ptr >= mbase && ptr < mbase + mbytes)
	{
		if (((size_t)(ptr - mbase) % MEDIUM_MALLOC) == 0)
			return MEDIUM;
	}
	return LARGE;
}