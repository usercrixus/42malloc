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

	if (pool->type == LARGE)
	{
		for (size_t i = 0; i < pool->slot_number; i++)
		{
			if (((void **)pool->pool)[i] == (void *)ptr)
				return i;
		}
	}
	else
	{
		const char *base = (const char *)pool->pool;
		const size_t bytes = pool->byte_size;
		const size_t stride = pool->byte_size / pool->slot_number; // real slot size
		if (ptr >= base && ptr < base + bytes)
			return ((size_t)(ptr - base) / stride);
	}
	return SIZE_MAX;
}

void *format_from_pool(size_t size, t_pool *pool)
{
	int id;
	size_t slotsSize;
	void *memoryPool;

	if (pool->type == LARGE)
	{
		size_t mlen = page_round_up(size);
		void *node = mmap(NULL, mlen, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		id = get_free_slot(pool);
		if (id == -1)
			return (NULL);
		pool->free[id] = mlen;
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

	for (size_t type = 0; type < POOL; type++)
	{
		size_t rows = g_malloc.pools_size[type]; // only initialized rows for this type

		for (size_t row = 0; row < rows; row++)
		{
			t_pool *p = &g_malloc.pools[row][type];

			if (!p->pool || p->slot_number == 0 || p->byte_size == 0)
				continue; // uninitialized row

			if (type == LARGE)
			{
				// LARGE: pool->pool is an array of pointers
				void *const *arr = (void *const *)p->pool;
				for (size_t i = 0; i < p->slot_number; i++)
				{
					if (arr[i] == (void *)ptr)
						return p;
				}
			}
			else
			{
				// Fixed-size pools: range + stride alignment
				const char *base = (const char *)p->pool;
				const size_t bytes = p->byte_size;
				if (ptr >= base && ptr < base + bytes)
				{
					const size_t stride = p->byte_size / p->slot_number; // safe: slot_number>0
					if (stride && ((size_t)(ptr - base) % stride) == 0)
						return p;
				}
			}
		}
	}
	return NULL;
}
