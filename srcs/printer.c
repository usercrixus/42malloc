#include <stdbool.h>
#include <stdlib.h>
#include "printer.h"
#include "malloc.h"

static size_t print_fixed_pool(const t_pool *pool, const char *label)
{
    size_t total = 0;
    const char *base_pool = (const char *)pool->pool;
    const size_t slot_size = pool->byte_size / pool->slot_number;
    ft_printf("%s: %p\n", label, base_pool);
    for (size_t i = 0; i < pool->slot_number; i++)
    {
        size_t used = pool->free[i];
        if (used)
        {
            const void *beg = base_pool + i * slot_size;
            const void *end = (const char *)beg + used;
            ft_printf("%p - %p : %d bytes\n", beg, end, used);
            total += used;
        }
    }
    return total;
}

static size_t print_large_pool(const t_pool *pool)
{
    size_t total = 0;
    void *const *base_pool = (void *const *)pool->pool;
    ft_printf("LARGE: %p\n", pool->pool);
    for (size_t i = 0; i < pool->slot_number; i++)
    {
        size_t used = pool->free[i];
        if (used)
        {
            const void *beg = base_pool[i];
            const void *end = (const char *)beg + used;
            ft_printf("%p - %p : %d bytes\n", beg, end, used);
            total += used;
        }
    }
    return total;
}

void show_alloc_mem(void)
{
    size_t total = 0;
    pthread_mutex_lock(&g_malloc.lock);
    for (size_t type = 0; type < POOL; type++)
    {
        for (size_t row = 0; row < g_malloc.pools_size[type]; row++)
        {
            const t_pool *pool = &g_malloc.pools[type][row];
            if (type == LARGE)
                total += print_large_pool(pool);
            else
                total += print_fixed_pool(pool, POOL_NAME[type]);
        }
    }
    ft_printf("TOTAL: %d bytes\n", total);
    pthread_mutex_unlock(&g_malloc.lock);
}

void print_memory_dump(void *ptr, size_t size)
{
    const unsigned char *p = (const unsigned char *)ptr;
    ft_printf("====DUMP====\n");
    for (size_t i = 0; i < size; i++)
    {
        ft_printf("x%X", p[i]);
        if ((i + 1) % 128 == 0)
            ft_printf("\n");
    }
    ft_printf("\n");
}

void print_panic(char *str)
{
    ft_printf("%s\n", str);
    abort();
}
