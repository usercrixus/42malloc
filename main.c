#include "srcs/malloc.h"
#include "srcs/printer.h"
#include <stdio.h>
#include <string.h>

void doFree(void *p)
{
	free(p);
}

int main()
{
	char *model = "Hello world";
	int *ar1 = malloc(sizeof(int) * 10);	  // 4 * 10 = 40 SMALL_MALLOC
	int *ar2 = malloc(sizeof(double) * 100);  // 8 * 100 = 800 MEDIUM_MALLOC
	int *ar3 = malloc(sizeof(double) * 200);  // 8 * 200 = 1600 MEDIUM_MALLOC
	int *ar4 = malloc(sizeof(double) * 1024); // too large no pool
	int *ar5 = malloc(sizeof(double) * 1028); // too large no pool
	char *s = malloc(sizeof(char) * 12);
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
	showAllocMem();
	printMemoryDump(ar1, sizeof(int) * 10);
	doFree(ar1);
	doFree(ar2);
	doFree(ar3);
	doFree(ar4);
	doFree(ar5);
	doFree(s);

	//do_free(ar1);
	//do_free(ar3);
	doFree(ar4);
}
