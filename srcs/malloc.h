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
	int slot;
	int type;
} t_malloc __attribute__((aligned(16)));

typedef struct s_mmap_entry
{
	void *ptr;
	struct s_mmap_entry *next;
} t_mmap_entry;

typedef struct s_reserved
{
	void *small;
	void *medium;
	size_t small_byte_size;
	size_t small_slot_size;
	size_t medium_byte_size;
	size_t medium_slot_size;
	short *free_small;
	short *free_medium;
	t_mmap_entry mmap_large_entries;
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