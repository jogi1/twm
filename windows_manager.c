#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "windows.h"
#include "windows_functions.h"
#include "monitor_functions.h"
#include "rules_manager.h"
#include "layout_manager.h"
#include "tools.h"

static struct window *managed_windows[128]; 
static int managed_windows_count;

void hotkey_windows_manage(void)
{
	static int managed = 0;

	if (managed == 0)
	{
		Layout_Apply();
	}
	else
	{
		Layout_Restore();
	}
	
	managed = !managed;

}


int Windows_Manager_Init(void)
{
	struct window *w;

	managed_windows_count = 0;

	// Reading in all windows
	if (Window_Readin_Windows())
		return 1;

	w = Window_Get_First();

	while (w)
	{
		if (Rule_Check_Window(w))
		{
			managed_windows[managed_windows_count++] = w;
		}
		w = w->next;
	}

	return 0;
}


int Windows_Manager_Quit(void)
{
	int i;

	for (i=0; i<managed_windows_count; i++)
		Window_Restore_Original(managed_windows[i]);

	return 0;
}

