#pragma once

#include <sys/types.h>
#include "malloc.h"

/**
 * return a free ptr from the pool and reserve it (mark as not free anymore)
 */
void *get_ptr_from_pool(size_t size, t_pool *pool);