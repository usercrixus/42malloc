#pragma once

#include <sys/types.h>
#include "malloc.h"

/**
 * Return a struct with the pool where the ptr is located and the id in this ptr in this pool
 */
t_pool_id get_pool_id(const char *ptr);