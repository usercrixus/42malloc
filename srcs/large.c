#include "large.h"

void mmap_large_entries_add(void *base, size_t size)
{
	pthread_mutex_lock(&g_malloc.lock);
	t_mmap_entry *cur = &g_malloc.reserved_memory.mmap_large_entries;
	while (cur->next)
		cur = cur->next;
	size_t mlen = page_round_up(sizeof(t_mmap_entry));
	t_mmap_entry *node = mmap(NULL, mlen, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (node == MAP_FAILED)
		print_panic("node == MAP_FAILED");
	node->ptr = base;
	node->size = size;
	node->next = NULL;
	cur->next = node;
	pthread_mutex_unlock(&g_malloc.lock);
}

size_t mmap_large_entries_remove(void *base)
{
	t_mmap_entry *prev = &g_malloc.reserved_memory.mmap_large_entries;
	t_mmap_entry *cur = prev->next;
	while (cur)
	{
		if (cur->ptr == base)
		{
			size_t size = cur->size;
			prev->next = cur->next;
			if (munmap(cur, page_round_up(sizeof(t_mmap_entry))) == -1)
				print_panic("munmap(cur, page_round_up(sizeof(t_mmap_entry))) == -1");
			return size;
		}
		prev = cur;
		cur = cur->next;
	}
	return 0;
}

size_t mmap_large_entries_get_size(void *base)
{
	t_mmap_entry *prev = &g_malloc.reserved_memory.mmap_large_entries;
	t_mmap_entry *cur = prev->next;
	while (cur)
	{
		if (cur->ptr == base)
			return cur->size;
		prev = cur;
		cur = cur->next;
	}
	return 0;
}
