#include "get_ptr_from_pool.h"
#include "init.h"

static size_t get_free_slot(t_pool *pool)
{
	pthread_mutex_lock(&g_malloc.lock);
	for (size_t i = 0; i < pool->slot_number; i++)
	{
		if ((pool->free)[i] == 0)
		{
			(pool->free)[i] = 1;
			pthread_mutex_unlock(&g_malloc.lock);
			return (i);
		}
	}
	pthread_mutex_unlock(&g_malloc.lock);
	return (SIZE_MAX);
}

static void *get_ptr_from_large_pool(size_t size, t_pool *pool)
{
	size_t id = get_free_slot(pool);
	if (id == SIZE_MAX)
		return (NULL);
	size = page_round_up(size);
	void *node = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (node == MAP_FAILED)
		return (NULL);
	pool->free[id] = size;
	((void **)(pool->pool))[id] = node;
	return node;
}

static void *get_ptr_from_normal_pool(size_t size, t_pool *pool)
{
	size_t id = get_free_slot(pool);
	if (id == SIZE_MAX)
		return (NULL);
	(pool->free)[id] = size;
	return (char *)(pool->pool) + (id * (pool->byte_size / pool->slot_number));
}

void *get_ptr_from_pool(size_t size, t_pool *pool)
{
	if (pool->type == LARGE)
		return (get_ptr_from_large_pool(size, pool));
	return (get_ptr_from_normal_pool(size, pool));
}