#pragma once

#include <sys/types.h>
#include "malloc.h"

size_t get_slot_id(const char *ptr);
void *format_from_pool(size_t size, int type);
int which_pool(const char *ptr);