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

void init_pool(t_pool *pool, size_t ALLOC_COUNT, size_t MALLOC, size_t type)
{
	pool->byte_size = page_round_up(ALLOC_COUNT * MALLOC);
	pool->slot_number = pool->byte_size / MALLOC; // MALLOC = pool->byte_size / pool->slot_number;
	pool->pool = mmap(NULL, pool->byte_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	pool->free = mmap(NULL, pool->slot_number * sizeof(size_t), PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	pool->type = type;
	if (pool->pool == MAP_FAILED || pool->free == MAP_FAILED)
		print_panic("*pool == MAP_FAILED || *free == MAP_FAILED");
	size_t i = 0;
	while (i < pool->slot_number)
		(pool->free)[i++] = 0;
}

t_pool (*init_pool_list(size_t bytes))[POOL]
{
	t_pool(*reserved_memory)[POOL];
	size_t byte_size = page_round_up(bytes);
	reserved_memory = mmap(NULL, byte_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (reserved_memory == MAP_FAILED)
		print_panic("g_malloc.reserved_memory == MAP_FAILED");
	g_malloc.pool_size = byte_size / sizeof(t_pool[POOL]);
	return reserved_memory;
}

static void init_thread()
{
	if (pthread_mutex_init(&g_malloc.lock, NULL) != 0)
		print_panic("pthread_mutex_init(&g_malloc.lock, NULL) != 0");
}

__attribute__((constructor)) static void init_malloc(void)
{
	size_t *unit_size = UNIT_SIZE;
	size_t *unit_numer = UNIT_NUMBER;
	init_debug();
	init_page_size();
	g_malloc.pools = init_pool_list(1);
	size_t i = 0;
	while (i < POOL)
	{
		init_pool(&g_malloc.pools[0][i], unit_numer[i], unit_size[i], i);
		g_malloc.pools_size[i] = 1;
		i++;
	}
	init_thread();
}