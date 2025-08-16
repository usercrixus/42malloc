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

#define SMALL_MALLOC 1024
#define MEDIUM_MALLOC 4096
#define LARGE_MALLOC sizeof(void *)
#define SMALL_ALLOC_COUNT 16384
#define MEDIUM_ALLOC_COUNT 16384
#define LARGE_ALLOC_COUNT 16384
#define SMALL 0
#define MEDIUM 1
#define LARGE 2

typedef struct s_pool
{
	void *pool;
	size_t byte_size;
	size_t slot_number;
	size_t *free;
	size_t remaining;
} t_pool;

typedef struct s_reserved
{
	t_pool small;
	t_pool medium;
	t_pool large;
} t_reserved;

typedef struct s_global
{
	short show_allocations;
	int fail_after;
	t_reserved reserved_memory;
	size_t page_size;
	pthread_mutex_t lock;
} t_global;

extern t_global g_malloc;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);