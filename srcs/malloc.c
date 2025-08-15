#include "malloc.h"

t_global g_malloc;


static void defragment(void *ptr, size_t size)
{
	ft_bzero(ptr, size);
}

void *malloc(size_t size)
{
	void *block;
	void *ptr;
	static _Atomic int alloc_count;

	if (g_malloc.fail_after >= 0 && alloc_count++ >= g_malloc.fail_after)
		return NULL;
	if (size == 0)
		return NULL;
	if (g_malloc.show_allocations)
		ft_printf("[malloc] Allocating 0x%x bytes\n", size);
	ptr = 0;
	if (size <= SMALL_MALLOC)
		ptr = format_from_pool(size, SMALL);
	else if (size <= MEDIUM_MALLOC)
		ptr = format_from_pool(size, MEDIUM);
	if (ptr)
		return ptr;
	block = mmap(NULL, page_round_up(size), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block == MAP_FAILED)
		return NULL;
	mmap_large_entries_add(block, size);
	return (char *)block;
}

void free(void *ptr)
{
	if (ptr)
	{
		int pool = which_pool(ptr);
		if (pool != LARGE)
		{
			size_t *free_referencies = pool == SMALL ? g_malloc.reserved_memory.free_small : g_malloc.reserved_memory.free_medium;
			size_t free_referencies_id = get_slot_id(ptr);
			size_t size = free_referencies[free_referencies_id];
			pthread_mutex_lock(&g_malloc.lock);
			if (free_referencies[free_referencies_id] == 0)
				print_panic("free_referencies[free_referencies_id] == 0");
			free_referencies[free_referencies_id] = 0;
			defragment(ptr, size);
			pthread_mutex_unlock(&g_malloc.lock);
		}
		else
		{
			pthread_mutex_lock(&g_malloc.lock);
			size_t size = mmap_large_entries_remove(ptr);
			if (!size)
				return (pthread_mutex_unlock(&g_malloc.lock), (void)0); // print_panic("!size");
			size_t map_len = page_round_up(size);
			defragment(ptr, map_len);
			if (munmap(ptr, map_len) == -1)
				print_panic("munmap(ptr, map_len) == -1");
			pthread_mutex_unlock(&g_malloc.lock);
		}
	}
}

void *realloc(void *ptr, size_t newSize)
{
	if (!ptr)
		return malloc(newSize);
	if (newSize == 0)
		return (free(ptr), NULL);

	int pool = which_pool(ptr);
	if (pool != LARGE)
	{
		size_t capacity = pool == SMALL ? SMALL_MALLOC : MEDIUM_MALLOC;
		if (newSize <= capacity)
		{
			if (pool == SMALL)
			{
				if (g_malloc.reserved_memory.free_small[get_slot_id(ptr)] == 0)
					return NULL;
				g_malloc.reserved_memory.free_small[get_slot_id(ptr)] = newSize;
			}
			else
			{
				if (g_malloc.reserved_memory.free_medium[get_slot_id(ptr)] == 0)
					return NULL;
				g_malloc.reserved_memory.free_medium[get_slot_id(ptr)] = newSize;
			}
		}
		return (ptr);
	}
	size_t oldSize = mmap_large_entries_get_size(ptr);
	if (oldSize == 0)
		return (NULL);
	void *new_ptr = malloc(newSize);
	if (!new_ptr)
		return NULL;
	ft_memcpy(new_ptr, ptr, oldSize < newSize ? oldSize : newSize);
	free(ptr);
	return new_ptr;
}
