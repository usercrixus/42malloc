#include "srcs/malloc.h"
#include "srcs/printer.h"
#include <stdio.h>
#include <string.h>

void do_free(void *p)
{
	free(p);
}

int main()
{
	char *model = "Hello world";
	int *ar1 = malloc(sizeof(int) * 10);	  // 4 * 10 = 40 SMALL_MALLOC
	int *ar2 = malloc(sizeof(double) * 100);  // 8 * 100 = 800 MEDIUM_MALLOC
	int *ar3 = malloc(sizeof(double) * 200);  // 8 * 200 = 1600 MEDIUM_MALLOC
	int *ar4 = malloc(sizeof(double) * 1024); // 8 * 1024 = 8192 too large no pool
	int *ar5 = malloc(sizeof(double) * 1028); // 8 * 1028 = 8224 too large no pool
	char *s = malloc(sizeof(char) * 12);	  // 1 * 12 = 12 SMALL_MALLOC (realloc after)

	ft_printf("\n\nBasics test:\n\n");
	for (int i = 0; i < 10; i++)
	{
		ar1[i] = i;
	}
	strcpy(s, model);
	ft_printf("%s", s);
	ft_printf("\n");
	s = realloc(s, 200); // 200 byte !
	ft_printf("%s", s);
	ft_printf("\n\nMandatory test:\n\n");

	show_alloc_mem();
	// 40+800+1600+8192+8224+200 = 19056
	//print_memory_dump(ar1, sizeof(int) * 10);

	do_free(ar1);
	do_free(ar2);
	do_free(ar3);
	do_free(ar4);
	do_free(ar5);
	do_free(s);

	// do_free(ar1);
	// do_free(ar3);
	// do_free(ar4);
}
