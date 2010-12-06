#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "windows.h"
#include "windows_functions.h"
#include "monitor_functions.h"
#include "layout_manager.h"
#include "rules_manager.h"
#include "tools.h"

struct layout *layout_list, *layout_list_last;

struct layout *current_layout;

static struct layout *layout_add(void)
{
	if (layout_list == NULL)
	{
		layout_list = calloc(1, sizeof(struct layout));
		layout_list_last = layout_list;
		return layout_list;
	}

	layout_list_last->next = calloc(1, sizeof(struct layout));

	if (layout_list_last->next == NULL)
		return NULL;

	layout_list_last->next->prev = layout_list_last;

	layout_list_last = layout_list_last->next;

	return layout_list_last;
}

static struct layout *layout_find_by_name (char *name)
{
	struct layout *l;

	l = layout_list;

	while(l)
	{
		if (strcmp(l->name, name) == 0)
			return l;
		l = l->next;
	}
	return NULL;
}


struct layout *Layout_Create(char *name)
{
	struct layout *l;

	if (name == NULL)
	{
		printf("Layout_Create: name was null.\n");
		return NULL;
	}

	l = layout_add();

	if (l == NULL)
	{
		printf("Layout_Create: layout_add failed.\n");
		return NULL;
	}

	l->name = strdup(name);

	if (l->name == NULL)
	{
		free(l);
		printf("Layout_Create: strdup(name) failed/\n");
		return NULL;
	}

	return l;
}

int Layout_Set_Monitor(struct layout *l, int monitor)
{
	if (l == NULL)
		return 1;

	l->monitor = monitor;

	return 0;
}

static struct section *section_add(struct layout *l, char *name)
{
	struct section **osp, *sp;
	int i;
	char *n;

	if (l == NULL || name == NULL)
		return NULL;

	n = strdup(name);

	if (n == NULL)
		return NULL;

	if (l->section_count == 0)
	{
		l->section = calloc(1, sizeof(struct section *));
		if (l->section == NULL)
		{
			free(n);
			return NULL;
		}

		sp = l->section[0] = calloc(1, sizeof(struct section));
		if (l->section[0] == NULL)
		{
			free(n);
			free(l->section);
			return NULL;
		}
	}
	else
	{
		osp = l->section;

		l->section = calloc(l->section_count + 1, sizeof(struct section *));
		
		if (l->section == NULL)
		{
			free(n);
			l->section = osp;
			return NULL;
		}

		sp = l->section[l->section_count] = calloc(1, sizeof(struct section));
		
		if (l->section[l->section_count] == NULL)
		{
			free(l->section);
			free(n);
			l->section = osp;
			return NULL;
		}

		for (i=0; i<l->section_count; i++)
			l->section[i] = osp[i];

		free(osp);
	}

	l->section_count++;

	sp->name = n;

	return sp;
}

struct section *Layout_Add_Section(struct layout *l, char *name)
{
	struct section *s;

	if (l == NULL || name == NULL)
		return NULL;

	s = section_add(l, name);

	return s;
}

int Layout_Section_Set_Dimension_Float(struct section *s, float x, float y, float w, float h)
{
	if (s == NULL)
		return 1;

	printf("%f %f %f %f\n", x, y, w, h);
	s->fx = x;
	s->fy = y;
	s->fw = w;
	s->fh = h;

	return 0;
}

int Layout_Section_Calculate_Dimensions(struct section *s, struct layout *l)
{
	int mx, my, mw, mh;

	if (s == NULL || l == NULL)
		return 1;

	if (Monitor_Get_Dimensions(l->monitor, &mx, &my, &mw, &mh))
		return 1;

	s->ix = mx + mw * s->fx;
	s->iy = my + mh * s->fy;
	s->iw = mw * s->fw;
	s->ih = mh * s->fh;

	return 0;
}

static int add_window_to_section(struct section *s, struct window *w)
{
	struct window **owl;
	int i;

	if (s->windows_attached_count == 0)
	{
		s->windows_attached = calloc(1, sizeof(struct window *));
		if (s->windows_attached == NULL)
			return 1;

		s->windows_attached[0] = w;
	}
	else
	{
		owl = calloc(s->windows_attached_count + 1, sizeof(struct window *));
		if (owl == NULL)
			return 1;

		for (i=0; i<s->windows_attached_count; i++)
			owl[i] = s->windows_attached[i];

		owl[s->windows_attached_count] = w;

		free(s->windows_attached);
		s->windows_attached = owl;
	}

	s->windows_attached_count++;
	return 0;
}

int Layout_Section_Add_Window(struct section *s, struct window *w)
{
	add_window_to_section(s, w);
	return 0;
}

int Layout_Init(void)
{
	struct layout *l;
	struct section *s;

	l = Layout_Create("coding");
	if (l == NULL)
		return 1;

	s = Layout_Add_Section(l, "vim");
	if (s == NULL)
		return 1;

	Layout_Section_Set_Dimension_Float(s, 1.0f/3.0f, 0, 2.0f/3.0f, 1);
	Layout_Section_Calculate_Dimensions(s, l);

	if(Rule_Add("vim", s, ".*GVIM", "Xming X"))
	{
		printf("problem compiling vim regexp.\n");
		return 1;
	}

	s = Layout_Add_Section(l, "terms");
	if (s == NULL)
		return 1;

	Layout_Section_Set_Dimension_Float(s, 0, 0, 1.0f/3.0f, 1);
	Layout_Section_Calculate_Dimensions(s, l);


	if(Rule_Add("terms", s, "Administrator@jogi|jogi@debian-crosscompile", "Xming X"))
	{
		printf("problem compiling terms regexp.\n");
		return 1;
	}

	current_layout = l;

	l = Layout_Create("irc media");
	if (l == NULL)
		return 1;

	Layout_Set_Monitor(l, 1);

	s = Layout_Add_Section(l, "irc");
	if (s == NULL)
		return 1;

	Layout_Section_Set_Dimension_Float(s, 0, 0, 1.0f, 1.0f/3.5f);
	Layout_Section_Calculate_Dimensions(s, l);

	if (Rule_Add("irc", s, "jogi@mephhisto", "Xming X"))
	{
		printf("problem compiling irc regexp.\n");
		return 1;
	}

	s = Layout_Add_Section(l, "media");
	if (s == NULL)
		return 1;

	Layout_Section_Set_Dimension_Float(s, 0, 1.0f/3.5f, 1, 2.5f/3.5f);
	Layout_Section_Calculate_Dimensions(s, l);

	if (Rule_Add("media", s, "MPlayer", "MPlayer"))
	{
		printf("problem compiling media regexp.\n");
		return 1;
	}



	return 0;
}


void Layout_Section_Restore_Windows(struct section *s)
{
	int i;
	float f;

	f = (float)s->ih/(float)s->windows_attached_count;

	for (i=0; i<s->windows_attached_count; i++)
		Window_Restore_Original(s->windows_attached[i]);
}

void Layout_Section_Setup_Windows(struct section *s)
{
	int i;
	float f;

	if (s == NULL)
		return;

	if (s->windows_attached == 0)
		return;

	f = (float)s->ih/(float)s->windows_attached_count;

	for (i=0; i<s->windows_attached_count; i++)
	{
		Window_Set_Style(s->windows_attached[i], WS_POPUP | WS_VISIBLE);
		Window_Set_Dimensions(s->windows_attached[i], s->ix, s->iy + f * i, s->iw, f, 1);
	}
}

void Layout_Apply(void)
{
	struct layout *l;
	int i;

	l = layout_list;

	while (l)
	{
		for (i=0; i<l->section_count; i++)
			Layout_Section_Setup_Windows(l->section[i]);
		l = l->next;
	}
}

void Layout_Restore(void)
{
	struct layout *l;
	int i;

	l = layout_list;

	while (l)
	{
		for (i=0; i<l->section_count; i++)
			Layout_Section_Restore_Windows(l->section[i]);
		l = l->next;
	}
}

static void activate_section(struct section *s)
{
	SetForegroundWindow(s->windows_attached[s->windows_active_window]->handle);
}

int Layout_Section_Next_Section(struct layout *l)
{
	if (l == NULL)
		return 1;

	l->section_active++;

	if (l->section_active >= l->section_count)
		l->section_active = 0;

	activate_section(l->section[l->section_active]);

	return 0;
}

int Layout_Section_Previous_Section(struct layout *l)
{
	if (l == NULL)
		return 1;

	l->section_active--;

	if (l->section_active < 0)
		l->section_active = l->section_count - 1;

	activate_section(l->section[l->section_active]);

	return 0;
}

int Layout_Section_Next_Window(struct layout *l)
{
	struct section *s;

	if (l == NULL)
		return 1;

	s = l->section[l->section_active];

	s->windows_active_window++;

	if (s->windows_active_window <= s->windows_attached_count)
		s->windows_active_window = 0;

	activate_section(s);

	return 0;
}

int Layout_Section_Previous_Window(struct layout *l)
{	
	struct section *s;

	if (l == NULL)
		return 1;

	s = l->section[l->section_active];

	s->windows_active_window--;

	if (s->windows_active_window < 0)
		s->windows_active_window = s->windows_attached_count - 1;

	activate_section(s);

	return 0;
}
