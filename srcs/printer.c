#include "printer.h"

void printPool()
{
	int i;

	i = 0;
	ft_printf("====SMALL====\n");
	while (i < SMALL_ALLOC_COUNT)
	{
		void *block = (char *)g_malloc_reserved_memory.small + (i * SMALL_MALLOC);
		ft_printf("Memory: %p, size: %d\n", block, ((t_malloc *)block)->size);
		i++;
	}
	ft_printf("====MEDIUM====\n");
	i = 0;
	while (i < MEDIUM_ALLOC_COUNT)
	{
		void *block = (char *)g_malloc_reserved_memory.medium + (i * MEDIUM_MALLOC);
		ft_printf("Memory: %p, size: %d\n", block, ((t_malloc *)block)->size);
		i++;
	}
}

void printAllocMemEx(void *ptr, size_t size)
{
	int i;
	int j;

	ft_printf("====DUMP====\n");
	i = 0;
	j = 0;
	char *charPtr = (char *)ptr;
	while (i < size)
	{
		ft_printf("x%X", charPtr[i]);
		i++;
		if (i % 128 == 0)
			ft_printf("\n");
	}
	ft_printf("\n");
}