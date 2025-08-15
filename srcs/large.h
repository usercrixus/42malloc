#pragma once

#include <sys/types.h>
#include "malloc.h"

void mmap_large_entries_add(void *base, size_t size);
size_t mmap_large_entries_remove(void *base);
size_t mmap_large_entries_get_size(void *base);