MAKEFLAGS += --no-print-directory

OBJ_SHARED = \
	srcs/malloc.o \
	srcs/printer.o \
	srcs/init.o \
	srcs/large.o \
	srcs/medium.o \

OBJ_MAIN = \
	main.o \

OBJ_MAIN2 = \
	main2.o \

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

main1.out: libft $(OBJ_MAIN)
	gcc -Wall -Werror -Wextra $(OBJ_MAIN) -L. -lft_malloc -o $@

main2.out: libft $(OBJ_MAIN2)
	gcc -Wall -Werror -Wextra $(OBJ_MAIN2) -L. -lft_malloc -o $@

%.o: %.c
	gcc -Wall -Werror -Wextra -fpic -c $< -o $@

test-crash:
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=1 MYMALLOC_FAIL_AFTER=1 ./main1.out

test1: all main1.out
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=0 ./main1.out


testFinal:
	gcc tester/test0.c -L. -lft_malloc -o 0
	gcc tester/test1.c -L. -lft_malloc -o 1
	gcc tester/test2.c -L. -lft_malloc -o 2
	gcc tester/test3.c -L. -lft_malloc -o 3
	gcc tester/test3bis.c -L. -lft_malloc -o 3b
	gcc tester/test4.c -L. -lft_malloc -o 4
	gcc tester/test4bis.c -L. -lft_malloc -o 4b
	gcc tester/test5.c -L. -lft_malloc -o 5
	gcc  main1.c -L. -lft_malloc -o main1.out
	gcc  main2.c -L. -lft_malloc -o main2.out

	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so /usr/bin/time -v ./0
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so /usr/bin/time -v ./1
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so /usr/bin/time -v ./2
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./3
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./3b
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./4
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./4b
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so ./5
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=0 ./main2.out
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=0 ./main1.out

	rm -f 0 1 2 3 3b 4 4b 5 main2.out main1.out

clean:
	make -C srcs/42libft clean
	rm -f $(OBJ)

fclean: clean
	rm -f main1.out main2.out
	rm -f libft_malloc_*.so libft_malloc.so

re: fclean all

.PHONY: clean fclean submodule re test test-crash

