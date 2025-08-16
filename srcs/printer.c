#include "printer.h"
#include "malloc.h"

static int print_pool(t_pool *pool)
{
    int total = 0;
    char *base = (char *)pool->pool;
    size_t slotsz = pool->byte_size / pool->slot_number;
    size_t n = pool->slot_number;
    ft_printf("SMALL: %p\n", base);
    for (size_t i = 0; i < n; i++)
    {
        size_t used = (pool->free)[i];
        if (used)
        {
            void *beg = base + i * slotsz;
            void *end = (char *)beg + used;
            ft_printf("%p - %p : %d bytes\n", beg, end, used);
            total += used;
        }
    }
    return total;
}
static int print_large()
{
    int total = 0;
    ft_printf("LARGE: %p\n", g_malloc.reserved_memory.large);
    for (size_t i = 0; i < g_malloc.reserved_memory.large.slot_number; i++)
    {
        size_t used = (g_malloc.reserved_memory.large.free)[i];
        if (used)
        {
            void *beg = ((void **)(g_malloc.reserved_memory.large.pool))[i];
            void *end = (char *)beg + used;
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
    total += print_pool(&(g_malloc.reserved_memory.small));
    total += print_pool(&(g_malloc.reserved_memory.medium));
    total += print_large();
    ft_printf("TOTAL: %d bytes\n", total);
    pthread_mutex_unlock(&g_malloc.lock);
}

void print_memory_dump(void *ptr, size_t size)
{
    size_t i = 0;
    char *char_ptr = (char *)ptr;
    ft_printf("====DUMP====\n");
    while (i < size)
    {
        ft_printf("x%X", char_ptr[i]);
        i++;
        if (i % 128 == 0)
            ft_printf("\n");
    }
    ft_printf("\n");
}

void print_panic(char *str)
{
    ft_printf("%s\n", str);
    abort();
}