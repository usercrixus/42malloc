#pragma once

#include <stddef.h>
#include "42libft/ft_printf/ft_printf.h"
#include "malloc.h"

void show_alloc_mem();
void print_memory_dump(void *ptr, size_t size);
void print_panic(char *str);