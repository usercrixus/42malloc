#include <stdbool.h>
#include <stdlib.h>
#include "printer.h"
#include "malloc.h"

static size_t print_fixed_pool(const t_pool *pool, const char *label)
{
    size_t total = 0;
    const char *base = (const char *)pool->pool;
    if (!base || pool->slot_number == 0) return 0;

    const size_t slot_size = pool->byte_size / pool->slot_number;

    bool printed_header = false;
    for (size_t i = 0; i < pool->slot_number; i++)
    {
        size_t used = pool->free[i];
        if (used)
        {
            if (!printed_header)
            {
                ft_printf("%s: %p\n", label, pool->pool);
                printed_header = true;
            }
            const void *beg = base + i * slot_size;
            const void *end = (const char *)beg + used;
            ft_printf("%p - %p : %zu bytes\n", beg, end, (size_t)used);
            total += used;
        }
    }
    return total;
}

static size_t print_large_pool(const t_pool *pool)
{
    size_t total = 0;
    if (!pool->pool || pool->slot_number == 0) return 0;

    bool printed_header = false;
    void *const *ptr_array = (void *const *)pool->pool;

    for (size_t i = 0; i < pool->slot_number; i++)
    {
        size_t used = pool->free[i];
        if (used)
        {
            if (!printed_header)
            {
                ft_printf("LARGE: %p\n", pool->pool);
                printed_header = true;
            }
            const void *beg = ptr_array[i];
            const void *end = (const char *)beg + used;
            ft_printf("%p - %p : %zu bytes\n", beg, end, (size_t)used);
            total += used;
        }
    }
    return total;
}

/*
** Public API
*/

void show_alloc_mem(void)
{
    size_t total = 0;

    pthread_mutex_lock(&g_malloc.lock);

    // SMALL
    for (size_t i = 0; i < g_malloc.pools_size[SMALL]; i++)
        total += print_fixed_pool(&g_malloc.pools[i][SMALL], "SMALL");

    // MEDIUM
    for (size_t i = 0; i < g_malloc.pools_size[MEDIUM]; i++)
        total += print_fixed_pool(&g_malloc.pools[i][MEDIUM], "MEDIUM");

    // LARGE
    for (size_t i = 0; i < g_malloc.pools_size[LARGE]; i++)
        total += print_large_pool(&g_malloc.pools[i][LARGE]);

    ft_printf("TOTAL: %zu bytes\n", total);

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
