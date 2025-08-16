#include "malloc.h"
#include "medium.h"
#include "init.h"
#include "printer.h"

t_global g_malloc;
 
void extend_pool()
{
	pthread_mutex_lock(&g_malloc.lock);
	size_t previous_reserved_memory_size = g_malloc.reserved_memory_size;
	t_reserved *previous_reserved_memory = g_malloc.reserved_memory;
	t_reserved *new_reserved_memory = init_pool_list(previous_reserved_memory_size * sizeof(t_reserved) * 2);
	ft_memcpy(new_reserved_memory, previous_reserved_memory, previous_reserved_memory_size * sizeof(t_reserved));
	g_malloc.reserved_memory = new_reserved_memory;
	munmap(previous_reserved_memory, previous_reserved_memory_size);
	size_t i = previous_reserved_memory_size;
	while (i < g_malloc.reserved_memory_size)
	{
		init_pool(&(g_malloc.reserved_memory[i].small), SMALL_ALLOC_COUNT, SMALL_MALLOC);
		init_pool(&(g_malloc.reserved_memory[i].medium), MEDIUM_ALLOC_COUNT, MEDIUM_MALLOC);
		init_pool(&(g_malloc.reserved_memory[i].large), LARGE_ALLOC_COUNT, LARGE_MALLOC);
		i++;
	}
	pthread_mutex_unlock(&g_malloc.lock);
}

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
	size_t i = 0; 
	while (ptr == NULL && i < g_malloc.reserved_memory_size)
	{
		if (size <= SMALL_MALLOC)
			ptr = format_from_pool(size, &(g_malloc.reserved_memory[i].small));
		else if (size <= MEDIUM_MALLOC)
			ptr = format_from_pool(size, &(g_malloc.reserved_memory[i].medium));
		else
			ptr = format_from_pool(size, &(g_malloc.reserved_memory[i].large));
		if (g_malloc.show_allocations && ptr)
		{
			pthread_mutex_lock(&g_malloc.lock);
			ft_printf("[malloc] Allocating 0x%x bytes\n", size);
			pthread_mutex_unlock(&g_malloc.lock);
		}
		i++;
		if (i == g_malloc.reserved_memory_size && ptr == NULL)
			extend_pool();
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
		if (pool->is_large)
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
