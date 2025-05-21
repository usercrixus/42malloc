MAKEFLAGS += --no-print-directory

OBJ_SHARED = \
	srcs/malloc.o \
	srcs/printer.o \

OBJ_MAIN = \
	main.o \

all: submodule main.out

main.out: libft $(OBJ_MAIN)
	gcc -Wall -Werror -Wextra $(OBJ_MAIN) -L. -lft_malloc -o $@

libft:
	make -C srcs/42libft

 %.o: %.c
	gcc -Wall -Werror -Wextra -fpic -c $< -o $@

submodule:
	git submodule update --init --recursive

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

so: libft_malloc_$(HOSTTYPE).so libft_malloc.so

libft_malloc_$(HOSTTYPE).so: $(OBJ_SHARED) libft
	gcc -shared srcs/malloc.o srcs/printer.o \
		./srcs/42libft/ft_base/libft.a \
		./srcs/42libft/ft_printf/libftprintf.a \
		-o libft_malloc_$(HOSTTYPE).so

libft_malloc.so: libft_malloc_$(HOSTTYPE).so
	ln -sf libft_malloc_$(HOSTTYPE).so libft_malloc.so

clean-so:
	rm -f libft_malloc_*.so libft_malloc.so

test-crash:
	MYMALLOC_SHOW_ALLOCATIONS=1 MYMALLOC_FAIL_AFTER=1 ./main.out

run-so: so main.out
	LD_LIBRARY_PATH=. LD_PRELOAD=./libft_malloc.so MYMALLOC_SHOW_ALLOCATIONS=1 ./main.out

clean:
	make -C srcs/42libft clean
	rm -f $(OBJ)

fclean: clean
	rm -f main.out

re: fclean all

.PHONY: clean fclean submodule re

