#include "printer.h"

void show_alloc_mem(void)
{
	void *base;
	t_malloc *hdr;
	size_t total;

	pthread_mutex_lock(&g_malloc_lock);
	total = 0;
	base = g_malloc_reserved_memory.small;
	ft_printf("SMALL: %p\n", base);
	for (size_t i = 0; i < g_malloc_reserved_memory.small_slot_size; i++)
	{
		if (g_malloc_reserved_memory.free_small[i] == 0)
		{
			hdr = (t_malloc *)((char *)base + i * SMALL_MALLOC);
			total += hdr->size;
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
	}
	base = g_malloc_reserved_memory.medium;
	ft_printf("MEDIUM: %p\n", base);
	for (size_t i = 0; i < g_malloc_reserved_memory.medium_slot_size; i++)
	{
		if (g_malloc_reserved_memory.free_medium[i] == 0)
		{
			hdr = (t_malloc *)((char *)base + i * MEDIUM_MALLOC);
			total += hdr->size;
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
	}
	ft_printf("LARGE:\n");
	t_mmap_entry *cur = g_malloc_reserved_memory.mmap_large_entries.next; // skip sentinel
	while (cur)
	{
		t_malloc *hdr = (t_malloc *)cur->ptr; // base of mapping (header)
		if (hdr)
		{
			total += hdr->size;
			void *user = (char *)hdr + sizeof(t_malloc);
			ft_printf("%p - %p : %d bytes\n", user, (char *)user + hdr->size, hdr->size);
		}
		cur = cur->next;
	}
	ft_printf("TOTAL: %d bytes\n", total);
	pthread_mutex_unlock(&g_malloc_lock);
}

void print_memory_dump(void *ptr, size_t size)
{
	size_t i;

	ft_printf("====DUMP====\n");
	char *char_ptr = (char *)ptr;
	i = 0;
	while (i < size)
	{
		ft_printf("x%X", char_ptr[i]);
		i++;
		if (i % 128 == 0)
			ft_printf("\n");
	}
	ft_printf("\n");
}