#include"srcs/malloc.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[])
{
	char *model = "Hello world";
	int *ar1 = ft_malloc(sizeof(int) * 10);
	char* s = ft_malloc(sizeof(char) * 10);
	int *ar2 = ft_malloc(sizeof(double) * 100);
	int *ar3 = ft_malloc(sizeof(double) * 200);
	for (size_t i = 0; i < 10; i++)
	{
		ar1[i] = i;
	}
	for (size_t i = 0; i < ft_strlen(model); i++)
	{
		s[i] = model[i];
	}

	for (size_t i = 0; i < 10; i++)
	{
		ft_printf("%d ", ar1[i]);
	}
	ft_printf("\n");
	for (size_t i = 0; i < ft_strlen(model); i++)
	{
		ft_printf("%c", s[i]);
	}
	ft_printf("\n");
	printPool();
}
