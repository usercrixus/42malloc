#pragma once

#include <stdlib.h>
#include <sys/mman.h>
#define _GNU_SOURCE
#include <sys/types.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include "42libft/ft_base/libft.h"
#include "42libft/ft_printf/ft_printf.h"

/**
 * Here you should inform on your pool structure
 * It should have only one large pool, placed at the end of the structure.
 * Large should always be POOL - 1
 */
#define POOL 3
#define SMALL 0
#define MEDIUM 1
#define LARGE 2
#define TYPE {SMALL, MEDIUM, LARGE}
#define UNIT_SIZE ((size_t[]){1024, 4096, sizeof(void *)})
#define UNIT_NUMBER ((size_t[]){1024, 1024, 64})
#define POOL_NAME ((const char *[]){"SMALL", "MEDIUM", "LARGE"})

typedef struct s_pool
{
	void *pool;			// a pool of pointer
	size_t byte_size;	// the total size of the pool
	size_t slot_number; // the number of pointer available in the pool
	size_t type;		// the type of the pool (SMALL, MEDIUM, LARGE...)
	size_t *used;		// list of used pointer (0 for free, else, size asked by the user)
	size_t *free_ids;	// stack of free slot indices (LIFO)
	size_t free_top;	// count of items on the stack (index of next push)
} t_pool;

typedef struct s_global
{
	short show_allocations;		 // debug var, print when a malloc is done
	int fail_after;				 // debug var, fail after x malloc
	t_pool *pools[POOL];		 // and array of pool (one for each TYPE)
	size_t pools_capacity[POOL]; // how many rows allocated
	size_t pools_size[POOL];	 // how many rows are initialized
	size_t page_size;			 // the page size on this os
	pthread_mutex_t lock;		 // the malloc lock for thread safety
} t_global;

typedef struct s_pool_id
{
	size_t id;		  // id (rank) of the pointer in the pool (t_pool.pool)
	t_pool *pool;	  // the t_pool the pointer is in
	size_t parent_id; // the parent t_pool list offset (see t_pool *pools[POOL])
} t_pool_id;

extern t_global g_malloc;

void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
