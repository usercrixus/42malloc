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

static void init_thread()
{
	if (pthread_mutex_init(&g_malloc.lock, NULL) != 0)
		print_panic("pthread_mutex_init(&g_malloc.lock, NULL) != 0");
}

//-----------------------------------

static t_pool *alloc_pool_rows(size_t rows)
{
	size_t bytes = rows * sizeof(t_pool);
	bytes = page_round_up(bytes);
	void *new_pool = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (new_pool == MAP_FAILED)
		print_panic("alloc_pool_rows: mmap failed");
	return (new_pool);
}

static void free_pool_rows(t_pool *arr, size_t rows)
{
	if (arr && rows)
	{
		size_t bytes = page_round_up(rows * sizeof(t_pool));
		munmap(arr, bytes);
	}
}

static void extend_pool_type(size_t type)
{
	size_t old_capacity = g_malloc.pools_capacity[type];
	size_t new_capacity = old_capacity ? old_capacity * 2 : 1;
	t_pool *old_array = g_malloc.pools[type];
	t_pool *new_array = alloc_pool_rows(new_capacity);
	if (old_array && old_capacity)
		ft_memcpy(new_array, old_array, old_capacity * sizeof(t_pool));
	g_malloc.pools[type] = new_array;
	g_malloc.pools_capacity[type] = new_capacity;
	if (old_array && old_capacity)
		free_pool_rows(old_array, old_capacity);
}

static void init_pool(t_pool *pool, size_t ALLOC_COUNT, size_t MALLOC, size_t type)
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

void init_single_pool(size_t type)
{
	size_t row = g_malloc.pools_size[type];
	if (row >= g_malloc.pools_capacity[type])
		extend_pool_type(type);
	init_pool(&g_malloc.pools[type][row], UNIT_NUMBER[type], UNIT_SIZE[type], type);
	g_malloc.pools_size[type] = row + 1;
}

//------------------------------------------

__attribute__((constructor)) static void init_malloc(void)
{
	init_debug();
	init_page_size();
	init_thread();
	// per-type arrays: start with cap=1 and init row 0
	for (size_t type = 0; type < POOL; type++)
	{
		g_malloc.pools[type] = NULL;
		g_malloc.pools_capacity[type] = 0;
		g_malloc.pools_size[type] = 0;
		init_single_pool(type);
	}
}
