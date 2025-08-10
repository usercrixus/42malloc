#pragma once

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include "42libft/ft_base/libft.h"
#include "42libft/ft_printf/ft_printf.h"

#define SMALL_MALLOC 512
#define MEDIUM_MALLOC 4096
#define SMALL_ALLOC_COUNT 256
#define MEDIUM_ALLOC_COUNT 256
#define SMALL 0
#define MEDIUM 1

typedef struct s_malloc
{
	size_t size;
	int isFromPool;
	int slot;
	int type;
} t_malloc __attribute__((aligned(16)));

typedef struct s_reserved
{
	void *small;
	void *medium;
	size_t smallByteSize;
	size_t smallSlotSize;
	size_t mediumByteSize;
	size_t mediumSlotSize;
	short *freeSmall;
	short *freeMedium;
} t_reserved;

extern short g_malloc_show_allocations;
extern int g_malloc_fail_after;
extern t_reserved g_malloc_reserved_memory;
extern size_t g_malloc_page_size;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);