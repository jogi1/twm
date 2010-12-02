#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "tokenize_string.h"
#include "windows.h"

void get_size_from_rect(RECT* rect, int *x, int *y, int *width, int *height)
{
	if (rect == NULL || x == NULL || y == NULL || width == NULL || height == NULL)
		return;

	*x = rect->left;
	*y = rect->top;
	*width = rect->right - rect->left;
	*height = rect->bottom - rect->top;
}

int get_time (char *time_string, int time_string_size)
{
	time_t t;
	struct tm *ptm;

	time (&t);

	if ((ptm = localtime(&t)) == NULL)
	{
		printf("time failed :P\n");
		return 1;
	}

	strftime(time_string, time_string_size, "%H:%M:%S", ptm);
	return 0;
}


#if 0
int main (void)
{
	char time_string[32];
	int time_string_size;

	time_string_size = 32;

	if (get_time(time_string, time_string_size))
		return 1;

	printf("%s\n", time_string);
}
#endif
