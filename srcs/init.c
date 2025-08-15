#include "init.h"

size_t page_round_up(size_t sz)
{
	return ((sz + g_malloc.page_size - 1) / g_malloc.page_size) * g_malloc.page_size;
}

static void init_debug()
{
	char *env;

	g_malloc.show_allocations = 0;
	g_malloc.fail_after = -1;
	env = getenv("MYMALLOC_SHOW_ALLOCATIONS");
	if (env && atoi(env) == 1)
		g_malloc.show_allocations = 1;
	env = getenv("MYMALLOC_FAIL_AFTER");
	if (env)
		g_malloc.fail_after = atoi(env);
}

static void init_page_size()
{
	g_malloc.page_size = sysconf(_SC_PAGESIZE);
}

static void init_pool(size_t *byte_size, size_t *slot_size, void **pool, size_t **free, size_t ALLOC_COUNT, size_t MALLOC)
{
	*byte_size = page_round_up(ALLOC_COUNT * MALLOC);
	*slot_size = *byte_size / MALLOC;
	*pool = mmap(NULL, *byte_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	*free = mmap(NULL, *slot_size * sizeof(size_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (*pool == MAP_FAILED || *free == MAP_FAILED)
		print_panic("*pool == MAP_FAILED || *free == MAP_FAILED");
	size_t i = 0;
	while (i < *slot_size)
		(*free)[i++] = 0;
}

static void init_large()
{
	g_malloc.reserved_memory.large_byte_size = page_round_up(LARGE_ALLOC_COUNT * LARGE_MALLOC);
	g_malloc.reserved_memory.large_slot_size = g_malloc.reserved_memory.large_byte_size / LARGE_MALLOC;
	g_malloc.reserved_memory.large = mmap(NULL, g_malloc.reserved_memory.large_byte_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	g_malloc.reserved_memory.free_large = mmap(NULL, g_malloc.reserved_memory.large_slot_size * sizeof(size_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (g_malloc.reserved_memory.large == MAP_FAILED || g_malloc.reserved_memory.free_large == MAP_FAILED)
		print_panic("g_malloc.reserved_memory.large == MAP_FAILED || g_malloc.reserved_memory.free_large == MAP_FAILED");
	size_t i = 0;
	while (i < g_malloc.reserved_memory.large_slot_size)
		g_malloc.reserved_memory.free_large[i++] = 0;
}

static void init_thread()
{
	if (pthread_mutex_init(&g_malloc.lock, NULL) != 0)
		print_panic("pthread_mutex_init(&g_malloc.lock, NULL) != 0");
}

__attribute__((constructor)) static void init_malloc(void)
{
	init_debug();
	init_page_size();
	init_pool(&(g_malloc.reserved_memory.small_byte_size), &(g_malloc.reserved_memory.small_slot_size), &(g_malloc.reserved_memory.small), &(g_malloc.reserved_memory.free_small), SMALL_ALLOC_COUNT, SMALL_MALLOC);
	init_pool(&(g_malloc.reserved_memory.medium_byte_size), &(g_malloc.reserved_memory.medium_slot_size), &(g_malloc.reserved_memory.medium), &(g_malloc.reserved_memory.free_medium), MEDIUM_ALLOC_COUNT, MEDIUM_MALLOC);
	init_large();
	init_thread();
}

static void destroy_small()
{
	munmap(g_malloc.reserved_memory.small, g_malloc.reserved_memory.small_byte_size);
	munmap(g_malloc.reserved_memory.free_small, g_malloc.reserved_memory.small_slot_size * sizeof(size_t));
}

static void destroy_medium()
{
	munmap(g_malloc.reserved_memory.medium, g_malloc.reserved_memory.medium_byte_size);
	munmap(g_malloc.reserved_memory.free_medium, g_malloc.reserved_memory.medium_slot_size * sizeof(size_t));
}

static void destroy_large()
{
	munmap(g_malloc.reserved_memory.large, g_malloc.reserved_memory.large_byte_size);
	munmap(g_malloc.reserved_memory.free_large, g_malloc.reserved_memory.large_slot_size * sizeof(size_t));
}

__attribute__((destructor)) static void destroy_malloc()
{
	destroy_small();
	destroy_medium();
	destroy_large();
	pthread_mutex_destroy(&g_malloc.lock);
}
