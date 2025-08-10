#include "malloc.h"

t_reserved g_malloc_reserved_memory;
pthread_mutex_t g_malloc_lock;
short g_malloc_show_allocations = 0;
int g_malloc_fail_after = -1;
_Atomic int g_malloc_alloc_count = 0;
size_t g_malloc_page_size = 0;

static inline size_t page_round_up(size_t sz)
{
	return ((sz + g_malloc_page_size - 1) / g_malloc_page_size) * g_malloc_page_size;
}

__attribute__((constructor)) static void initMalloc(void)
{
	size_t i;
	char *env;

	env = getenv("MYMALLOC_SHOW_ALLOCATIONS");
	if (env && atoi(env) == 1)
		g_malloc_show_allocations = 1;
	env = getenv("MYMALLOC_FAIL_AFTER");
	if (env)
		g_malloc_fail_after = atoi(env);
	g_malloc_page_size = sysconf(_SC_PAGESIZE);
	g_malloc_reserved_memory.smallByteSize = page_round_up(SMALL_ALLOC_COUNT * SMALL_MALLOC);
	g_malloc_reserved_memory.smallSlotSize = g_malloc_reserved_memory.smallByteSize / SMALL_MALLOC;
	g_malloc_reserved_memory.small = mmap(NULL, g_malloc_reserved_memory.smallByteSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	g_malloc_reserved_memory.freeSmall = mmap(NULL, g_malloc_reserved_memory.smallSlotSize * sizeof(short), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_malloc_reserved_memory.small == MAP_FAILED || g_malloc_reserved_memory.freeSmall == MAP_FAILED)
		abort();
	g_malloc_reserved_memory.mediumByteSize = page_round_up(MEDIUM_ALLOC_COUNT * MEDIUM_MALLOC);
	g_malloc_reserved_memory.mediumSlotSize = g_malloc_reserved_memory.mediumByteSize / MEDIUM_MALLOC;
	g_malloc_reserved_memory.medium = mmap(NULL, g_malloc_reserved_memory.mediumByteSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	g_malloc_reserved_memory.freeMedium = mmap(NULL, g_malloc_reserved_memory.mediumSlotSize * sizeof(short), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_malloc_reserved_memory.medium == MAP_FAILED || g_malloc_reserved_memory.freeMedium == MAP_FAILED)
		abort();
	i = 0;
	while (i < g_malloc_reserved_memory.smallSlotSize)
		g_malloc_reserved_memory.freeSmall[i++] = 1;
	i = 0;
	while (i < g_malloc_reserved_memory.mediumSlotSize)
		g_malloc_reserved_memory.freeMedium[i++] = 1;
	if (pthread_mutex_init(&g_malloc_lock, NULL) != 0)
		ft_printf("mutex init has failed\n");
}

__attribute__((destructor)) static void destroyMalloc()
{
	munmap(g_malloc_reserved_memory.small, g_malloc_reserved_memory.smallByteSize);
	munmap(g_malloc_reserved_memory.medium, g_malloc_reserved_memory.mediumByteSize);
	munmap(g_malloc_reserved_memory.freeSmall, g_malloc_reserved_memory.smallSlotSize * sizeof(short));
	munmap(g_malloc_reserved_memory.freeMedium, g_malloc_reserved_memory.mediumSlotSize * sizeof(short));
	pthread_mutex_destroy(&g_malloc_lock);
}

int getFreeSlot(short *slot, size_t size)
{
	size_t i;

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

void *formatReturnFromPool(size_t size, int type)
{
	int id;
	size_t slotsSize;
	void *memoryPool;

	if (type == SMALL)
	{
		slotsSize = SMALL_MALLOC;
		memoryPool = g_malloc_reserved_memory.small;
		id = getFreeSlot(g_malloc_reserved_memory.freeSmall, g_malloc_reserved_memory.smallSlotSize);
	}
	else
	{
		slotsSize = MEDIUM_MALLOC;
		memoryPool = g_malloc_reserved_memory.medium;
		id = getFreeSlot(g_malloc_reserved_memory.freeMedium, g_malloc_reserved_memory.mediumSlotSize);
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

void *malloc(size_t size)
{
	void *block;
	void *ptr;

	if (g_malloc_fail_after >= 0 && g_malloc_alloc_count++ >= g_malloc_fail_after)
		return NULL;
	if (size > SIZE_MAX - sizeof(t_malloc))
		return NULL;
	if (size == 0)
		return NULL;
	if (g_malloc_show_allocations)
		ft_printf("[malloc] Allocating 0x%x bytes\n", size);
	size_t totalSize = size + sizeof(t_malloc);
	ptr = 0;
	if (totalSize <= SMALL_MALLOC)
		ptr = formatReturnFromPool(size, SMALL);
	else if (totalSize <= MEDIUM_MALLOC)
		ptr = formatReturnFromPool(size, MEDIUM);
	if (ptr)
		return ptr;
	block = mmap(NULL, page_round_up(totalSize), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
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

void free(void *ptr)
{
	if (ptr)
	{
		t_malloc *hdr = (t_malloc *)((char *)ptr - sizeof(t_malloc));
		if (hdr->isFromPool)
		{
			pthread_mutex_lock(&g_malloc_lock);
			(hdr->type == SMALL ? g_malloc_reserved_memory.freeSmall : g_malloc_reserved_memory.freeMedium)[hdr->slot] = 1;
			defragment(ptr, hdr->size);
			pthread_mutex_unlock(&g_malloc_lock);
		}
		else
		{
			defragment(ptr, hdr->size);
			munmap((void *)hdr, page_round_up(hdr->size + sizeof(t_malloc)));
		}
	}
}

void *realloc(void *ptr, size_t newSize)
{
	if (!ptr)
		return malloc(newSize);
	if (newSize == 0)
		return (free(ptr), NULL);

	t_malloc *hdr = (t_malloc *)((char *)ptr - sizeof(t_malloc));
	if (hdr->isFromPool)
	{
		size_t capacity = ((hdr->type == SMALL) ? SMALL_MALLOC : MEDIUM_MALLOC) - sizeof(t_malloc);
		if (newSize <= capacity)
			return (hdr->size = newSize, ptr);
	}

	size_t oldSize = hdr->size;
	void *new_ptr = malloc(newSize);
	if (!new_ptr)
		return NULL;
	size_t sizeToCpy = oldSize < newSize ? oldSize : newSize;
	ft_memcpy(new_ptr, ptr, sizeToCpy);
	free(ptr);
	return new_ptr;
}
