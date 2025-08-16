#include "medium.h"
#include "init.h"
#include "printer.h"

static int get_free_slot(t_pool *pool)
{
	size_t i;

	i = 0;
	pthread_mutex_lock(&g_malloc.lock);
	while (i < pool->slot_number)
	{
		if ((pool->free)[i] == 0)
		{
			(pool->free)[i] = 1;
			pthread_mutex_unlock(&g_malloc.lock);
			return (i);
		}
		i++;
	}
	pthread_mutex_unlock(&g_malloc.lock);
	return (-1);
}

size_t get_slot_id(const char *ptr, t_pool *pool)
{
	if (!ptr)
		print_panic("get_slot_id");
	if (pool->pool == g_malloc.reserved_memory.large.pool)
	{
		size_t i = 0;
		while (i < LARGE_ALLOC_COUNT)
		{
			if (((void **)(pool->pool))[i] == (void *)ptr)
				return (i);
			i++;
		}
	}
	else
	{
		const char *sbase = (const char *)pool->pool;
		const size_t sbytes = pool->byte_size;
		if (ptr >= sbase && ptr < sbase + sbytes)
			return ((size_t)(ptr - sbase) / SMALL_MALLOC);
	}
	return (SIZE_MAX);
}

void *format_from_pool(size_t size, t_pool *pool)
{
	int id;
	size_t slotsSize;
	void *memoryPool;

	if (pool->pool == g_malloc.reserved_memory.large.pool)
	{
		size_t mlen = page_round_up(size);
		void *node = mmap(NULL, mlen, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		id = get_free_slot(pool);
		if (id == -1)
			return (NULL);
		pool->free[id] = size;
		((void **)(pool->pool))[id] = node;
		return node;
	}
	else
	{
		slotsSize = pool->byte_size / pool->slot_number;
		memoryPool = pool->pool;
		id = get_free_slot(pool);
		if (id == -1)
			return (NULL);
		(pool->free)[id] = size;
		return (char *)memoryPool + (id * slotsSize);
	}
	return (NULL);
}

t_pool *which_pool(const char *ptr)
{
	if (!ptr)
		print_panic("which pool");
	const char *sbase = (const char *)g_malloc.reserved_memory.small.pool;
	const size_t sbytes = g_malloc.reserved_memory.small.byte_size;
	const char *mbase = (const char *)g_malloc.reserved_memory.medium.pool;
	const size_t mbytes = g_malloc.reserved_memory.medium.byte_size;

	if (ptr >= sbase && ptr < sbase + sbytes)
	{
		if (((size_t)(ptr - sbase) % SMALL_MALLOC) == 0)
			return &(g_malloc.reserved_memory.small);
	}
	if (ptr >= mbase && ptr < mbase + mbytes)
	{
		if (((size_t)(ptr - mbase) % MEDIUM_MALLOC) == 0)
			return &(g_malloc.reserved_memory.medium);
	}
	return &(g_malloc.reserved_memory.large);
}