#include "init.h"
#include "printer.h"

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

void init_pool(t_pool *pool, size_t ALLOC_COUNT, size_t MALLOC)
{
	pool->byte_size = page_round_up(ALLOC_COUNT * MALLOC);
	pool->slot_number = pool->byte_size / MALLOC; // MALLOC = pool->byte_size / pool->slot_number;
	pool->pool = mmap(NULL, pool->byte_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	pool->free = mmap(NULL, pool->slot_number * sizeof(size_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	pool->is_large = MALLOC == LARGE_MALLOC;
	if (pool->pool == MAP_FAILED || pool->free == MAP_FAILED)
		print_panic("*pool == MAP_FAILED || *free == MAP_FAILED");
	size_t i = 0;
	while (i < pool->slot_number)
		(pool->free)[i++] = 0;
}

t_reserved *init_pool_list(size_t size)
{
	t_reserved *reserved_memory;
	size_t byte_size = page_round_up(size);
	reserved_memory = mmap(NULL, byte_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (reserved_memory == MAP_FAILED)
		print_panic("g_malloc.reserved_memory == MAP_FAILED");
	g_malloc.reserved_memory_size = byte_size / sizeof(t_reserved);
	return reserved_memory;
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
	g_malloc.reserved_memory = init_pool_list(1);
	size_t i = 0;
	while (i < g_malloc.reserved_memory_size)
	{
		init_pool(&(g_malloc.reserved_memory[i].small), SMALL_ALLOC_COUNT, SMALL_MALLOC);
		init_pool(&(g_malloc.reserved_memory[i].medium), MEDIUM_ALLOC_COUNT, MEDIUM_MALLOC);
		init_pool(&(g_malloc.reserved_memory[i].large), LARGE_ALLOC_COUNT, LARGE_MALLOC);
		i++;
	}
	init_thread();
}

void destroy_pool(t_pool *pool)
{
	munmap(pool->pool, pool->byte_size);
	munmap(pool->free, pool->slot_number * sizeof(size_t));
}

void destroy_pool_list()
{
	munmap(g_malloc.reserved_memory, g_malloc.reserved_memory_size * sizeof(t_reserved));
}

__attribute__((destructor)) static void destroy_malloc()
{
	size_t i = 0;
	while (i < g_malloc.reserved_memory_size)
	{
		destroy_pool(&(g_malloc.reserved_memory[i].large));
		destroy_pool(&(g_malloc.reserved_memory[i].medium));
		destroy_pool(&(g_malloc.reserved_memory[i].small));
		i++;
	}
	destroy_pool_list();
	pthread_mutex_destroy(&g_malloc.lock);
}
