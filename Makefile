MAKEFLAGS += --no-print-directory

OBJ = \
	main.o \
	srcs/malloc.o \

all: main.out

main.out: libft $(OBJ)
	gcc -Wall -Werror -Wextra $(OBJ) -L./srcs/42libft/ft_base/ -lft -L./srcs/42libft/ft_printf/ -lftprintf -o $@

libft:
	make -C srcs/42libft

 %.o: %.c
	gcc -c $< -o $@

submodule:
	git submodule update --init --recursive

clean:
	make -C srcs/42libft clean
	rm -f $(OBJ)

fclean: clean
	rm -f main.out

re: fclean all

.PHONY: clean fclean submodule re