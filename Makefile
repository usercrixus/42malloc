MAKEFLAGS += --no-print-directory

OBJ = \
	main.o \
	srcs/malloc.o \
	srcs/printer.o \

all: submodule main.out

main.out: libft $(OBJ)
	gcc -Wall -Werror -Wextra $(OBJ) -fPIC -L./srcs/42libft/ft_base/ -lft -L./srcs/42libft/ft_printf/ -lftprintf -o $@

libft:
	make -C srcs/42libft

 %.o: %.c
	gcc -Wall -Werror -Wextra -fPIC -c $< -o $@

submodule:
	git submodule update --init --recursive

clean:
	make -C srcs/42libft clean
	rm -f $(OBJ)

fclean: clean
	rm -f main.out

re: fclean all

.PHONY: clean fclean submodule re

# ========== SUBJECT REQUIREMENTS ==========
ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

so: libft_malloc_$(HOSTTYPE).so libft_malloc.so

libft_malloc_$(HOSTTYPE).so: libft srcs/malloc.o srcs/printer.o
	gcc -shared -fPIC srcs/malloc.o srcs/printer.o \
		./srcs/42libft/ft_base/libft.a \
		./srcs/42libft/ft_printf/libftprintf.a \
		-o libft_malloc_$(HOSTTYPE).so

libft_malloc.so: libft_malloc_$(HOSTTYPE).so
	ln -sf libft_malloc_$(HOSTTYPE).so libft_malloc.so

clean-so:
	rm -f libft_malloc_*.so libft_malloc.so

test-crash:
	MYMALLOC_SHOW_ALLOCATIONS=1 MYMALLOC_FAIL_AFTER=1 ./main.out

test-verbose:
	MYMALLOC_SHOW_ALLOCATIONS=1 ./main.out

test:
	./main.out

run-so: main.out so
	LD_PRELOAD=./libft_malloc.so ./main.out