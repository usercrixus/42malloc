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
	gcc tester/test0.c -L. -lft_malloc -o 0
	gcc tester/test1.c -L. -lft_malloc -o 1
	gcc tester/test2.c -L. -lft_malloc -o 2
	gcc tester/test3.c -L. -lft_malloc -o 3
	gcc tester/test3bis.c -L. -lft_malloc -o 3b
	gcc tester/test4.c -L. -lft_malloc -o 4
	gcc tester/test4bis.c -L. -lft_malloc -o 4b
	gcc tester/test5.c -L. -lft_malloc -o 5
	gcc tester/test6.c -L. -lft_malloc -o 6
	gcc tester/test7.c -L. -lft_malloc -o 7

	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so /usr/bin/time -v ./0
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so /usr/bin/time -v ./1
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so /usr/bin/time -v ./2
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./3
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./3b
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./4
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./4b
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./5
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=1 ./6
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=0 ./7

	rm -f 0 1 2 3 3b 4 4b 5 6 7

clean:
	make -C srcs/42libft clean
	rm -f $(OBJ)

fclean: clean
	rm -f main1.out main2.out
	rm -f libft_malloc_*.so libft_malloc.so

re: fclean all

.PHONY: clean fclean submodule re test test-crash

