#include "malloc.h"

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
		ptr = format_from_pool(size, SMALL);
	else if (size <= MEDIUM_MALLOC)
		ptr = format_from_pool(size, MEDIUM);
	else
		ptr = format_from_pool(size, LARGE);
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
		int pool = which_pool(ptr);
		size_t *free_referencies;
		if (pool == SMALL)
			free_referencies = g_malloc.reserved_memory.free_small;
		else if (pool == MEDIUM)
			free_referencies = g_malloc.reserved_memory.free_medium;
		else
			free_referencies = g_malloc.reserved_memory.free_large;
		pthread_mutex_lock(&g_malloc.lock);
		size_t free_referencies_id = get_slot_id(ptr);
		size_t size = free_referencies[free_referencies_id];
		if (free_referencies[free_referencies_id] == 0)
		{
			pthread_mutex_unlock(&g_malloc.lock);
			return;
		}
		free_referencies[free_referencies_id] = 0;
		defragment(ptr, size);
		if (pool == LARGE)
			munmap(g_malloc.reserved_memory.large[free_referencies_id], size);
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
	size_t oldSize;
	int pool = which_pool(ptr);
	if (pool == SMALL)
		oldSize = g_malloc.reserved_memory.free_small[get_slot_id(ptr)];
	else if (pool == MEDIUM)
		oldSize = g_malloc.reserved_memory.free_medium[get_slot_id(ptr)];
	else
		oldSize = g_malloc.reserved_memory.free_large[get_slot_id(ptr)];
	if (oldSize == 0)
		return (pthread_mutex_unlock(&g_malloc.lock), NULL);
	ft_memcpy(new_ptr, ptr, oldSize < newSize ? oldSize : newSize);
	pthread_mutex_unlock(&g_malloc.lock);
	free(ptr);
	return new_ptr;
}
