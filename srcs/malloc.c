#include "get_pool_id.h"
#include "get_ptr_from_pool.h"
#include "init.h"
#include "malloc.h"
#include "printer.h"

t_global g_malloc;

static size_t get_pool_type(size_t size)
{
	for (size_t i = 0; i < POOL - 1; i++)
	{
		if (size <= UNIT_SIZE[i])
			return (i);
	}
	return (LARGE);
}

void *malloc(size_t size)
{
	static _Atomic int alloc_count;
	if (g_malloc.fail_after >= 0 && alloc_count++ >= g_malloc.fail_after)
		return (NULL);
	if (size == 0)
		return (NULL);
	void *ptr = NULL;
	size_t type = get_pool_type(size);
	for (size_t i = 0; !ptr && i < g_malloc.pools_size[type]; i++)
	{
		ptr = get_ptr_from_pool(size, &g_malloc.pools[type][i]);
		pthread_mutex_lock(&g_malloc.lock);
		if (g_malloc.show_allocations && ptr)
			ft_printf("[malloc] Allocating %d bytes\n", size);
		if (!ptr && i + 1 == g_malloc.pools_size[type])
			init_single_pool(type);
		pthread_mutex_unlock(&g_malloc.lock);
	}
	return (ptr);
}

static void defragment(t_pool_id *pid)
{
	if (pid->pool->free_top == pid->pool->slot_number)
	{
		if (pid->parent_id != g_malloc.pools_size[pid->pool->type] - 1)
		{
			t_pool tmp = g_malloc.pools[pid->pool->type][pid->parent_id];
			g_malloc.pools[pid->pool->type][pid->parent_id] = g_malloc.pools[pid->pool->type][g_malloc.pools_size[pid->pool->type] - 1];
			g_malloc.pools[pid->pool->type][g_malloc.pools_size[pid->pool->type] - 1] = tmp;
			// if we want to destroy (munmap) the empty pools... but we should have a strategy, as it is it an hard bottleneck.
			// my point... we should not delete them, if the program ever reach that point, it has lot of chance to reach that point again.
			// destroy_pool(pid->pool);
			// g_malloc.pools_size[pid->pool->type]--;
		}
	}
}

void free(void *ptr)
{
	if (!ptr)
		return;
	t_pool_id pid = get_pool_id(ptr);
	if (pid.pool == NULL || pid.id == SIZE_MAX)
		return;
	pthread_mutex_lock(&g_malloc.lock);
	size_t size = pid.pool->used[pid.id];
	if (pid.pool->type == LARGE)
	{
		void **arr = (void **)pid.pool->pool;
		if (arr[pid.id])
		{
			munmap(arr[pid.id], size);
			arr[pid.id] = NULL;
		}
	}
	pid.pool->used[pid.id] = 0;
	pid.pool->free_ids[pid.pool->free_top++] = pid.id;
	defragment(&pid);
	pthread_mutex_unlock(&g_malloc.lock);
}

static bool shrink(size_t newSize, size_t oldSize, size_t type, t_pool_id *pid)
{
	if (newSize <= oldSize)
	{
		if (type == LARGE)
		{
			void *base = ((void **)pid->pool->pool)[pid->id];
			size_t old_pg = page_round_up(oldSize);
			size_t new_pg = page_round_up(newSize);
			if (new_pg < old_pg)
				munmap((char *)base + new_pg, old_pg - new_pg);
		}
		pid->pool->used[pid->id] = newSize;
		return (true);
	}
	return (false);
}

static bool extend(size_t type, size_t newSize, t_pool_id *pid)
{
	if (type != LARGE && newSize <= UNIT_SIZE[type])
	{
		pid->pool->used[pid->id] = newSize;
		return true;
	}
	return false;
}

void *realloc(void *ptr, size_t newSize)
{
	if (!ptr)
		return malloc(newSize);
	if (newSize == 0)
		return (free(ptr), NULL);
	pthread_mutex_lock(&g_malloc.lock);
	t_pool_id pid = get_pool_id(ptr);
	if (pid.pool == NULL || pid.id == SIZE_MAX)
		return (pthread_mutex_unlock(&g_malloc.lock), NULL);
	size_t oldSize = pid.pool->used[pid.id];
	size_t type = pid.pool->type;
	if (shrink(newSize, oldSize, type, &pid) || extend(type, newSize, &pid))
		return (pthread_mutex_unlock(&g_malloc.lock), ptr);
	pthread_mutex_unlock(&g_malloc.lock);
	void *new_ptr = malloc(newSize);
	if (!new_ptr)
		return NULL;
	ft_memcpy(new_ptr, ptr, oldSize < newSize ? oldSize : newSize);
	free(ptr);
	return new_ptr;
}
