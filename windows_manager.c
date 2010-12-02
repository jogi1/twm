#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "windows.h"
#include "windows_functions.h"
#include "monitor_functions.h"
#include "rules_manager.h"
#include "tools.h"



int Windows_Manager_Init(void)
{
	struct window *w;

	w = Window_Get_First();

	while (w)
	{
		Rule_Check_Window(w);
		w = w->next;
	}

	return 0;
}


int Windows_Manager_Quit(void)
{

	return 0;
}
