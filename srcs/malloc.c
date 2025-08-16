#include "malloc.h"
#include "medium.h"
#include "init.h"
#include "printer.h"

t_global g_malloc;

static void defragment(void *ptr, size_t size)
{
	ft_bzero(ptr, size);
}

void *malloc(size_t size)
{
	void *ptr;
	static _Atomic int alloc_count;

	if (g_malloc.fail_after >= 0 && alloc_count++ >= g_malloc.fail_after)
		return NULL;
	if (size == 0)
		return NULL;
	ptr = NULL;
	if (size <= SMALL_MALLOC)
		ptr = format_from_pool(size, &(g_malloc.reserved_memory.small));
	else if (size <= MEDIUM_MALLOC)
		ptr = format_from_pool(size, &(g_malloc.reserved_memory.medium));
	else
		ptr = format_from_pool(size, &(g_malloc.reserved_memory.large));
	if (g_malloc.show_allocations && ptr)
	{
		pthread_mutex_lock(&g_malloc.lock);
		ft_printf("[malloc] Allocating 0x%x bytes\n", size);
		pthread_mutex_unlock(&g_malloc.lock);
	}
	return ptr;
}

void free(void *ptr)
{
	if (ptr)
	{
		t_pool *pool = which_pool(ptr);
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
		pool->remaining++;
		if (pool->pool == g_malloc.reserved_memory.large.pool)
			munmap(((void **)(g_malloc.reserved_memory.large.pool))[id], size);
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
