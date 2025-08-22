MAKEFLAGS += --no-print-directory

OBJ_SHARED = \
	srcs/get_pool_id.o \
	srcs/get_ptr_from_pool.o \
	srcs/init.o \
	srcs/malloc.o \
	srcs/printer.o \

HDR_SHARED = \
	srcs/get_pool_id.h \
	srcs/get_ptr_from_pool.h \
	srcs/init.h \
	srcs/malloc.h \
	srcs/printer.h \

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

all: submodule so

submodule:
	git submodule update --init --recursive

libft:
	make -C srcs/42libft

so: libft_malloc_$(HOSTTYPE).so libft_malloc.so

libft_malloc_$(HOSTTYPE).so: $(OBJ_SHARED) libft
	gcc -shared $(OBJ_SHARED) \
		./srcs/42libft/ft_base/libft.a \
		./srcs/42libft/ft_printf/libftprintf.a \
		-o libft_malloc_$(HOSTTYPE).so

libft_malloc.so: libft_malloc_$(HOSTTYPE).so
	ln -sf libft_malloc_$(HOSTTYPE).so libft_malloc.so

%.o: %.c $(HDR_SHARED)
	gcc -Wall -Werror -Wextra -fpic -O3 -c $< -o $@

test:
	gcc tester/test.c -L. -lft_malloc -o tester.out
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=0 ./tester.out

	rm tester.out

clean:
	make -C srcs/42libft clean
	rm -f $(OBJ)

fclean: clean
	rm -f libft_malloc_*.so libft_malloc.so

re: fclean all

.PHONY: clean fclean submodule re

