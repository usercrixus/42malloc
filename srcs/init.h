#pragma once

#include <sys/types.h>
#include "malloc.h"

size_t page_round_up(size_t sz);
void init_pool(t_pool *pool, size_t ALLOC_COUNT, size_t MALLOC);
void destroy_pool(t_pool *pool);