#pragma once

#include <sys/types.h>
#include "malloc.h"

size_t page_round_up(size_t sz);
void init_pool(t_pool *pool, size_t ALLOC_COUNT, size_t MALLOC, size_t type);
t_pool (*init_pool_list(size_t bytes))[POOL];