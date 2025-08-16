#pragma once

#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "42libft/ft_base/libft.h"
#include "42libft/ft_printf/ft_printf.h"

#define POOL 3
#define SMALL 0
#define MEDIUM 1
#define LARGE 2
#define TYPE {SMALL, MEDIUM, LARGE}
#define UNIT_SIZE ((size_t[]){1024, 4096, sizeof(void *)})
#define UNIT_NUMBER ((size_t[]){32, 32, 32})

typedef struct s_pool
{
	void *pool;
	size_t byte_size;
	size_t slot_number;
	size_t *free;
	size_t type;
} t_pool;

typedef struct s_global
{
	short show_allocations;
	int fail_after;
	t_pool (*pools)[POOL];
	size_t pool_size;
	size_t pools_size[POOL];
	size_t page_size;
	pthread_mutex_t lock;
} t_global;

extern t_global g_malloc;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
