#include <stdbool.h>
#include <stdlib.h>
#include "printer.h"
#include "malloc.h"

static size_t print_fixed_pool(const t_pool *pool, const char *label)
{
    size_t total = 0;
    const char *base = (const char *)pool->pool;
    if (!base || pool->slot_number == 0)
        return 0;
    const size_t slot_size = pool->byte_size / pool->slot_number;
    ft_printf("%s: %p\n", label, pool->pool);
    for (size_t i = 0; i < pool->slot_number; i++)
    {
        size_t used = pool->free[i];
        if (used)
        {
            const void *beg = base + i * slot_size;
            const void *end = (const char *)beg + used;
            ft_printf("%p - %p : %d bytes\n", beg, end, (size_t)used);
            total += used;
        }
    }
    return total;
}

static size_t print_large_pool(const t_pool *pool)
{
    size_t total = 0;
    if (!pool->pool || pool->slot_number == 0)
        return 0;
    void *const *ptr_array = (void *const *)pool->pool;
    ft_printf("LARGE: %p\n", pool->pool);
    for (size_t i = 0; i < pool->slot_number; i++)
    {
        size_t used = pool->free[i];
        if (used)
        {
            const void *beg = ptr_array[i];
            const void *end = (const char *)beg + used;
            ft_printf("%p - %p : %d bytes\n", beg, end, (size_t)used);
            total += used;
        }
    }
    return total;
}

void show_alloc_mem(void)
{
    const char **pool_name = POOL_NAME;
    size_t total = 0;
    pthread_mutex_lock(&g_malloc.lock);

    for (size_t j = 0; j < POOL - 1; j++)
    {
        for (size_t i = 0; i < g_malloc.pools_size[j]; i++)
            total += print_fixed_pool(&g_malloc.pools[i][j], pool_name[j]);
    }
    for (size_t i = 0; i < g_malloc.pools_size[LARGE]; i++)
        total += print_large_pool(&g_malloc.pools[i][LARGE]);
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
