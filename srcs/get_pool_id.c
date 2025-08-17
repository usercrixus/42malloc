#include "get_pool_id.h"
#include "init.h"
#include "printer.h"

static size_t get_large_pool_id(const char *ptr, t_pool *pool)
{
	void *const *base_pool = (void *const *)pool->pool;
	for (size_t i = 0; i < pool->slot_number; i++)
		if (base_pool[i] == (void *)ptr)
			return (i);
	return (SIZE_MAX);
}

static size_t get_normal_pool_id(const char *ptr, t_pool *pool)
{
	const char *base = (const char *)pool->pool;
	const size_t bytes = pool->byte_size;
	if (ptr >= base && ptr < base + bytes)
	{
		const size_t stride = pool->byte_size / pool->slot_number;
		if ((size_t)(ptr - base) % stride == 0)
			return ((size_t)(ptr - base) / stride);
	}
	return (SIZE_MAX);
}

t_pool_id get_pool_id(const char *ptr)
{
	if (!ptr)
		print_panic("which pool");
	t_pool_id pool_id;
	for (size_t type = 0; type < POOL; type++)
	{
		for (size_t i = 0; i < g_malloc.pools_size[type]; i++)
		{
			t_pool *pool = &(g_malloc.pools[type][i]);
			pool_id.id = type == LARGE ? get_large_pool_id(ptr, pool) : get_normal_pool_id(ptr, pool);
			if (pool_id.id != SIZE_MAX)
				return (pool_id.pool = pool, pool_id);
		}
	}
	pool_id.pool = NULL;
	pool_id.id = SIZE_MAX;
	return (pool_id);
}
