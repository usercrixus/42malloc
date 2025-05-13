#include "printer.h"

void show_alloc_mem(void)
{
	void *base;
	t_malloc *hdr;

	base = g_malloc_reserved_memory.small;
	ft_printf("SMALL: %p\n", base);
	for (int i = 0; i < SMALL_ALLOC_COUNT; i++)
	{
		if (g_malloc_reserved_memory.freeSmall[i] == 0)
		{
			hdr = (t_malloc *)((char *)base + i * SMALL_MALLOC);
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
	}
	base = g_malloc_reserved_memory.medium;
	ft_printf("MEDIUM: %p\n", base);
	for (int i = 0; i < MEDIUM_ALLOC_COUNT; i++)
	{
		if (g_malloc_reserved_memory.freeMedium[i] == 0)
		{
			hdr = (t_malloc *)((char *)base + i * MEDIUM_MALLOC);
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
	}
}

void printMemoryDump(void *ptr, size_t size)
{
	size_t i;

	ft_printf("====DUMP====\n");
	char *charPtr = (char *)ptr;
	i = 0;
	while (i < size)
	{
		ft_printf("x%X", charPtr[i]);
		i++;
		if (i % 128 == 0)
			ft_printf("\n");
	}
	ft_printf("\n");
}