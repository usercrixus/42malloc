#include "printer.h"

void show_alloc_mem(void)
{
	void *base;
	t_malloc *hdr;

	pthread_mutex_lock(&g_malloc_lock);
	base = g_malloc_reserved_memory.small;
	ft_printf("SMALL: %p\n", base);
	for (size_t i = 0; i < g_malloc_reserved_memory.smallSlotSize; i++)
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
	for (size_t i = 0; i < g_malloc_reserved_memory.mediumSlotSize; i++)
	{
		if (g_malloc_reserved_memory.freeMedium[i] == 0)
		{
			hdr = (t_malloc *)((char *)base + i * MEDIUM_MALLOC);
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
	}
	ft_printf("LARGE:\n");
	t_mmap_entry *cur = g_malloc_reserved_memory.mmap_entries.next; // skip sentinel
	while (cur)
	{
		t_malloc *hdr = (t_malloc *)cur->ptr; // base of mapping (header)
		if (hdr)
		{
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
		cur = cur->next;
	}
	pthread_mutex_unlock(&g_malloc_lock);
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