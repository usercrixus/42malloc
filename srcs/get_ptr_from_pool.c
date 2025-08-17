#include "get_ptr_from_pool.h"
#include "init.h"

static size_t pop_free_slot(t_pool *pool)
{
	pthread_mutex_lock(&g_malloc.lock);
	if (pool->free_top == 0)
	{
		pthread_mutex_unlock(&g_malloc.lock);
		return SIZE_MAX;
	}
	size_t id = pool->free_ids[--pool->free_top];
	pthread_mutex_unlock(&g_malloc.lock);
	return id;
}

static void *get_ptr_from_large_pool(size_t size, t_pool *pool)
{
	size_t id = pop_free_slot(pool);
	if (id == SIZE_MAX)
		return NULL;
	size_t mlen = page_round_up(size);
	void *node = mmap(NULL, mlen, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (node == MAP_FAILED)
	{
		pthread_mutex_lock(&g_malloc.lock);
		pool->free_ids[pool->free_top++] = id;
		pthread_mutex_unlock(&g_malloc.lock);
		return NULL;
	}
	pthread_mutex_lock(&g_malloc.lock);
	pool->used[id] = mlen;
	((void **)pool->pool)[id] = node;
	pthread_mutex_unlock(&g_malloc.lock);

	return node;
}

static void *get_ptr_from_normal_pool(size_t size, t_pool *pool)
{
	size_t id = pop_free_slot(pool);
	if (id == SIZE_MAX)
		return NULL;
	const size_t stride = pool->byte_size / pool->slot_number;
	char *ptr = (char *)pool->pool + id * stride;
	pthread_mutex_lock(&g_malloc.lock);
	pool->used[id] = size;
	pthread_mutex_unlock(&g_malloc.lock);
	return ptr;
}

void *get_ptr_from_pool(size_t size, t_pool *pool)
{
	if (pool->type == LARGE)
		return get_ptr_from_large_pool(size, pool);
	return get_ptr_from_normal_pool(size, pool);
}
