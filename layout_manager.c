#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "windows.h"
#include "windows_functions.h"
#include "monitor_functions.h"
#include "tools.h"

struct section
{
	float f_tl[2], f_br[2];
	int i_tl[2], i_br[2];
	char *name;
	int windows_attached_count;
	struct window **windows_attached;
	struct window *active_window;
};

struct layout
{
	int monitor;
	int section_count;
	struct section **section;
};


