#include "malloc.h"
#include "medium.h"
#include "init.h"
#include "printer.h"

t_global g_malloc;

void extend_pool(void)
{
	ft_printf("--------------------- TEST extend_pool -------------------\n");
	pthread_mutex_lock(&g_malloc.lock);

	size_t old_rows = g_malloc.pool_size;
	size_t old_bytes = old_rows * sizeof(t_pool[POOL]);

	t_pool(*old_mem)[POOL] = g_malloc.pools;
	t_pool(*new_mem)[POOL] = init_pool_list(old_bytes * 2);

	ft_memcpy(new_mem, old_mem, old_bytes);
	g_malloc.pools = new_mem;
	munmap(old_mem, old_bytes);

	pthread_mutex_unlock(&g_malloc.lock);
}

void init_single_pool(size_t type)
{
	//ft_printf("--------------------- TEST init_single_pool -------------------\n");
	const size_t *unit_size = UNIT_SIZE;
	const size_t *unit_number = UNIT_NUMBER;
	size_t row = g_malloc.pools_size[type];
	if (row >= g_malloc.pool_size)
		extend_pool();
	init_pool(&g_malloc.pools[row][type], unit_number[type], unit_size[type], type);
	g_malloc.pools_size[type] = row + 1;
}

static void defragment(void *ptr, size_t size)
{
	ft_bzero(ptr, size);
}

size_t get_pool_type(size_t size)
{
	const size_t *unit_size = UNIT_SIZE;
	for (size_t i = 0; i < POOL - 1; i++)
	{
		if (size <= unit_size[i])
			return i;
	}
	return LARGE;
}

void *malloc(size_t size)
{
	static _Atomic int alloc_count;
	if (g_malloc.fail_after >= 0 && alloc_count++ >= g_malloc.fail_after)
		return NULL;
	if (size == 0)
		return NULL;
	void *ptr = NULL;
	size_t type = get_pool_type(size);
	size_t i = 0;
	while (ptr == NULL && i < g_malloc.pools_size[type])
	{
		ptr = format_from_pool(size, &g_malloc.pools[i][type]);
		if (g_malloc.show_allocations && ptr)
		{
			pthread_mutex_lock(&g_malloc.lock);
			ft_printf("[malloc] Allocating 0x%x bytes\n", size);
			pthread_mutex_unlock(&g_malloc.lock);
		}
		i++;
		if (i == g_malloc.pool_size && ptr == NULL)
			extend_pool();
		else if (i == g_malloc.pools_size[type] && ptr == NULL)
			init_single_pool(type);
	}
	return ptr;
}

void free(void *ptr)
{
	if (ptr)
	{
		t_pool *pool = which_pool(ptr);
		if (!pool)
			return;
		pthread_mutex_lock(&g_malloc.lock);
		size_t id = get_slot_id(ptr, pool);
		if (id == SIZE_MAX)
		{
			pthread_mutex_unlock(&g_malloc.lock);
			return;
		}
		size_t size = (pool->free)[id];
		if ((pool->free)[id] == 0)
		{
			pthread_mutex_unlock(&g_malloc.lock);
			return;
		}
		(pool->free)[id] = 0;
		defragment(ptr, size);
		if (pool->type == LARGE)
			munmap(((void **)(pool->pool))[id], size);
		pthread_mutex_unlock(&g_malloc.lock);
	}
}

void *realloc(void *ptr, size_t newSize)
{
	if (!ptr)
		return malloc(newSize);
	if (newSize == 0)
		return (free(ptr), NULL);
	void *new_ptr = malloc(newSize);
	if (!new_ptr)
		return NULL;
	pthread_mutex_lock(&g_malloc.lock);
	t_pool *pool = which_pool(ptr);
	if (pool == NULL)
		return NULL;
	size_t id = get_slot_id(ptr, pool);
	if (id == SIZE_MAX)
		return (pthread_mutex_unlock(&g_malloc.lock), NULL);
	size_t oldSize = pool->free[id];
	if (oldSize == 0)
		return (pthread_mutex_unlock(&g_malloc.lock), free(new_ptr), NULL);
	ft_memcpy(new_ptr, ptr, oldSize < newSize ? oldSize : newSize);
	pthread_mutex_unlock(&g_malloc.lock);
	free(ptr);
	return new_ptr;
}
