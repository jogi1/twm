#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "windows.h"
#include "windows_functions.h"
#include "tools.h"

struct monitor
{
	int active;
	int x, y, width, height;
};

static int monitor_count;

static struct monitor monitors[16]; 

static BOOL CALLBACK MonitorEnumProc (HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	LPRECT m;
	struct monitor *mon;

	if (monitor_count > 15)
		return FALSE;

	//printf("%p %p %p %p %i\n", hMonitor, hdcMonitor, lprcMonitor, dwData, i++);
	m = lprcMonitor;
	if (m == NULL)
		return FALSE;
	//	printf("%i %i %i %i\n", m->top, m->left, m->right, m->bottom);

	mon = &monitors[monitor_count];

	get_size_from_rect(lprcMonitor, &mon->x, &mon->y, &mon->width, &mon->height);

	printf("monitor%i: %i %i %i %i\n", monitor_count, mon->x, mon->y, mon->width, mon->height);


	monitor_count++;
	return TRUE;
}

int Monitor_Setup(void)
{
	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);

	if (monitor_count == 0)
		return 1;
	
	return 0;
}

int Monitor_Get_Dimensions(int monitor, int *x, int *y, int *width, int *height)
{
	if (monitor >= monitor_count)
		return 1;

	if (x)
		*x = monitors[monitor].x;
	if (y)
		*y = monitors[monitor].y;
	if (width)
		*width = monitors[monitor].width;
	if (height)
		*height = monitors[monitor].height;

	return 0;
}
