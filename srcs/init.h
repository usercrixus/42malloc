#pragma once

#include <sys/types.h>
#include "malloc.h"

/**
 * round a size to a page size
 */
size_t page_round_up(size_t size);
/**
 * init a new row of the selected type (used if no more slot are available for this type)
 */
void init_single_pool(size_t type);