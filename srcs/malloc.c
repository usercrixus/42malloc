#include "malloc.h"

t_reserved g_malloc_reserved_memory;
pthread_mutex_t g_malloc_lock;
int g_malloc_show_allocations = 0;
int g_malloc_fail_after = -1;
int g_malloc_alloc_count = 0;


static inline size_t page_round_up(size_t sz)
{
	size_t g_page_size;

	g_page_size  = sysconf(_SC_PAGESIZE);
	return ((sz + g_page_size - 1) / g_page_size) * g_page_size;
}

__attribute__((constructor)) static void initMalloc(void)
{
	int i;
	char *env;

	env = getenv("MYMALLOC_SHOW_ALLOCATIONS");
	if (env && atoi(env) == 1)
		g_malloc_show_allocations = 1;
	env = getenv("MYMALLOC_FAIL_AFTER");
	if (env)
	    g_malloc_fail_after = atoi(env);
	g_malloc_reserved_memory.small = mmap(NULL, page_round_up(SMALL_ALLOC_COUNT * SMALL_MALLOC), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_malloc_reserved_memory.small == MAP_FAILED)
		abort();
	g_malloc_reserved_memory.medium = mmap(NULL, page_round_up(MEDIUM_ALLOC_COUNT * MEDIUM_MALLOC), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_malloc_reserved_memory.medium == MAP_FAILED)
		abort();
	i = 0;
	while (i < SMALL_ALLOC_COUNT)
		g_malloc_reserved_memory.freeSmall[i++] = 1;
	i = 0;
	while (i < MEDIUM_ALLOC_COUNT)
		g_malloc_reserved_memory.freeMedium[i++] = 1;
	if (pthread_mutex_init(&g_malloc_lock, NULL) != 0)
		ft_printf("mutex init has failed\n"); 
}

__attribute__((destructor)) static void destroyMalloc()
{
	munmap(g_malloc_reserved_memory.small, page_round_up(SMALL_ALLOC_COUNT * SMALL_MALLOC));
	munmap(g_malloc_reserved_memory.medium, page_round_up(MEDIUM_ALLOC_COUNT * MEDIUM_MALLOC));
}

int getFreeSlot(int *slot, int size)
{
	int i;

	i = 0;
	pthread_mutex_lock(&g_malloc_lock);
	while (i < size)
	{
		if (slot[i] == 1)
		{
			slot[i] = 0;
			return (pthread_mutex_unlock(&g_malloc_lock), i);
		}
		i++;
	}
	return (pthread_mutex_unlock(&g_malloc_lock), -1);
}

void *formatReturnFromPool(int size, int type)
{
	int id;
	int slotsSize;
	void *memoryPool;
	int *slots;

	if (type == SMALL)
	{
		slotsSize = SMALL_MALLOC;
		slots = g_malloc_reserved_memory.freeSmall;
		memoryPool = g_malloc_reserved_memory.small;
		id = getFreeSlot(slots, SMALL_ALLOC_COUNT);
	}
	else
	{
		slotsSize = MEDIUM_MALLOC;
		slots = g_malloc_reserved_memory.freeMedium;
		memoryPool = g_malloc_reserved_memory.medium;
		id = getFreeSlot(slots, MEDIUM_ALLOC_COUNT);
	}
	if (id == -1)
		return NULL;
	void *block = (char *)memoryPool + (id * slotsSize);
	((t_malloc *)block)->size = size;
	((t_malloc *)block)->isFromPool = 1;
	((t_malloc *)block)->slot = id;
	((t_malloc *)block)->type = type;
	return (char *)block + sizeof(t_malloc);
}

void *ft_malloc(size_t size)
{
	void *block;
	void *ptr;

	if (g_malloc_fail_after >= 0 && g_malloc_alloc_count >= g_malloc_fail_after)
	    return NULL;
	g_malloc_alloc_count++;
	if (g_malloc_show_allocations)
	    ft_printf("[malloc] Allocating 0x%x bytes\n", size);
	if (size == 0)
		return NULL;
	size_t totalSize = size + sizeof(t_malloc);
	ptr = 0;
	if (totalSize <= SMALL_MALLOC)
		ptr = formatReturnFromPool(size, SMALL);
	else if (totalSize <= MEDIUM_MALLOC)
		ptr = formatReturnFromPool(size, MEDIUM);
	if (ptr)
		return ptr;
	block = mmap(g_malloc_reserved_memory.medium, totalSize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (block == MAP_FAILED)
		return NULL;
	((t_malloc *)block)->size = size;
	((t_malloc *)block)->isFromPool = 0;
	return (void *)((char *)block + sizeof(t_malloc));
}

void defragment(void *ptrStart, size_t size)
{
    ft_bzero(ptrStart, size);
}

void ft_free(void *ptr)
{
	if (!ptr)
		return;
	t_malloc *hdr = (t_malloc *)((char *)ptr - sizeof(t_malloc));
	if (hdr->isFromPool)
	{
		defragment(ptr, hdr->size);
		if (hdr->type == SMALL)
			g_malloc_reserved_memory.freeSmall[hdr->slot] = 1;
		else
			g_malloc_reserved_memory.freeMedium[hdr->slot] = 1;
	}
	else
	{
		defragment(ptr, hdr->size);
		munmap((void *)hdr, hdr->size + sizeof(t_malloc));
	}
}

void *ft_realloc(void *ptr, size_t new_size)
{
	if (!ptr)
		return ft_malloc(new_size);
	if (new_size == 0)
	{
		ft_free(ptr);
		return NULL;
	}
	t_malloc *hdr = (t_malloc *)((char *)ptr - sizeof(t_malloc));
	size_t old_size = hdr->size;
	void *new_ptr = ft_malloc(new_size);
	if (!new_ptr)
		return NULL;
	size_t to_copy = old_size < new_size ? old_size : new_size;
	ft_memcpy(new_ptr, ptr, to_copy);
	ft_free(ptr);
	return new_ptr;
}
