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
#include "medium.h"
#include "init.h"
#include "printer.h"

#define SMALL_MALLOC 1024
#define MEDIUM_MALLOC 4096
#define LARGE_MALLOC sizeof(void *)
#define SMALL_ALLOC_COUNT 16384
#define MEDIUM_ALLOC_COUNT 16384
#define LARGE_ALLOC_COUNT 16384
#define SMALL 0
#define MEDIUM 1
#define LARGE 2


typedef struct s_reserved
{
	void *small;
	void *medium;
	void **large;
	size_t small_byte_size;
	size_t small_slot_size;
	size_t medium_byte_size;
	size_t medium_slot_size;
	size_t large_byte_size;
	size_t large_slot_size;
	size_t *free_small;
	size_t *free_medium;
	size_t *free_large;
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