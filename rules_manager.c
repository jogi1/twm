#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "windows.h"
#include "windows_functions.h"
#include "monitor_functions.h"
#include "rules_manager.h"
#include "layout_manager.h"
#include "regexp.h"
#include "tools.h"

static struct rule *rule_list, *rule_list_last;

static struct rule *rule_add(void)
{
	if (rule_list == NULL)
	{
		rule_list = calloc(1, sizeof(struct window));
		rule_list_last = rule_list;
		return rule_list;
	}

	rule_list_last->next = calloc(1, sizeof(struct window));

	if (rule_list_last->next == NULL)
		return NULL;

	rule_list_last->next->prev = rule_list_last;

	rule_list_last = rule_list_last->next;

	return rule_list_last;
}

int Rule_Add(char *name, struct section *s, char *name_regexp, char *class_regexp)
{
	struct rule *r;

	r = rule_add();

	if (r == NULL)
		return 1;

	r->s = s;
	
	r->name_regexp = Regexp_Compile(name_regexp);
	if (r->name_regexp == NULL)
		return 1;

	r->class_regexp = Regexp_Compile(class_regexp);
	if (r->class_regexp == NULL)
		return 1;

	r->name = strdup(name);
	if (r->name == NULL)
		return 1;

	return 0;
}

int Rule_Check_Window(struct window *w)
{
	struct rule *r;
	int rc;

	r = rule_list;

	while (r)
	{
		if (( rc = Regexp_Match_String(r->name_regexp, w->window_name)) > 0)
		{
			if ((rc = Regexp_Match_String(r->class_regexp, w->class_name)) > 0)
			{
				printf("\"%s\" matched class and name\n", w->window_name);
				Layout_Section_Add_Window(r->s, w);
				return 1;
			}
		}
		r = r->next;
	}

	return 0;
}
