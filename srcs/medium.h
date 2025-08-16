#pragma once

#include <sys/types.h>
#include "malloc.h"

size_t get_slot_id(const char *ptr, t_pool *pool);
void *format_from_pool(size_t size, t_pool *pool);
t_pool *which_pool(const char *ptr);