#include "printer.h"

static int print_small()
{
    int total = 0;
    char *base = (char *)g_malloc.reserved_memory.small;
    size_t slotsz = SMALL_MALLOC;
    size_t n = g_malloc.reserved_memory.small_slot_size;
    ft_printf("SMALL: %p\n", base);
    for (size_t i = 0; i < n; i++)
    {
        size_t used = g_malloc.reserved_memory.free_small[i]; // 0 = free, >0 = bytes used
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

static int print_medium()
{
    int total = 0;
    char *base = (char *)g_malloc.reserved_memory.medium;
    size_t slotsz = MEDIUM_MALLOC;
    size_t n = g_malloc.reserved_memory.medium_slot_size;
    ft_printf("MEDIUM: %p\n", base);
    for (size_t i = 0; i < n; i++)
    {
        size_t used = g_malloc.reserved_memory.free_medium[i]; // 0 = free, >0 = bytes used
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
    for (size_t i = 0; i < g_malloc.reserved_memory.large_slot_size; i++)
    {
        size_t used = g_malloc.reserved_memory.free_large[i];
        if (used)
        {
            void *beg = g_malloc.reserved_memory.large[i];
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
    total += print_small();
    total += print_medium();
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