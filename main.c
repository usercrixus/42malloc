#include"srcs/malloc.h"
#include "srcs/printer.h"
#include <stdio.h>
#include <string.h>

int main()
{
	char *model = "Hello world";
	int *ar1 = malloc(sizeof(int) * 10);
	int *ar2 = malloc(sizeof(double) * 100);
	int *ar3 = malloc(sizeof(double) * 200);
	char* s = malloc(sizeof(char) * 12);
	for (int i = 0; i < 10; i++)
	{
		ar1[i] = i;
	}
	strcpy(s, model);
	ft_printf("%s", s);
	ft_printf("\n");
	s = realloc(s, 200);
	ft_printf("%s", s);
	ft_printf("\n");
	show_alloc_mem();
	printMemoryDump(ar1, sizeof(int) * 10);
	free(ar1);
	free(ar2);
	free(ar3);
	free(s);
}
