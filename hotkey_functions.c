#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include "tokenize_string.h"
#include "windows.h"

struct HotKey
{
	char *name;
	WORD modifiers;
	WORD key;
	void (*f)(void);
	struct HotKey *next, *prev;
};

static struct HotKey *hotkey_list, *hotkey_list_last;
static int hotkey_count = 1;

static void hotkey_remove(struct HotKey *h)
{

	if (h->prev)
	{
		h->prev->next = h->next;
	}

	if (h->next)
	{
		h->next->prev = h->prev;
	}

	if (h == hotkey_list_last)
	{
		hotkey_list_last = h->prev;
	}

	if (h == hotkey_list)
	{
		hotkey_list = h->next;
	}

	free(h);
}

static struct HotKey *hotkey_add(void)
{
	struct HotKey *h;

	h = calloc(1, sizeof(struct HotKey));
	
	if (h == NULL)
		return NULL;

	if (hotkey_list == NULL)
	{
		hotkey_list = h;
		hotkey_list_last = h;
		return h;
	}

	hotkey_list_last->next = h;
	h->prev = hotkey_list_last;

	hotkey_list_last = h;

	return h;
}

static struct HotKey *hotkey_find_by_name(char *string)
{
	return NULL;
}

int HotKey_Register(char *hotkey_combination, void (*function)(void))
{
	int i;
	struct tokenized_string *ts;
	struct HotKey *hotkey;

	if (hotkey_combination == NULL)
	{
		printf("hotkey_register: hotkey_combination was NULL\n");
		return 1;
	}

	if (function == NULL)
	{
		printf("hotkey_register: function was NULL\n");
		return 1;
	}

	ts = Tokenize_String(hotkey_combination);

	if (ts == NULL)
	{
		printf("hotkey_register: could not tokenize string.\n");
		return 1;
	}

	if (ts->count < 2)
	{
		printf("hotkey_register: hotkeys must be at least one modifier and one key.\n");
		Tokenize_String_Delete(ts);
		return 1;
	}

	hotkey = hotkey_add();
	
	if (hotkey == NULL)
	{
		printf("hotkey_register: hotkey_add failed.\n");
		Tokenize_String_Delete(ts);
		return 1;
	}

	for (i=0; i<ts->count; i++)
	{
		if (strlen(ts->tokens[i]) == 1)
		{
			hotkey->key = ts->tokens[i][0];
		}
		else
		{
			if (strcmp(ts->tokens[i], "shift") == 0)
				hotkey->modifiers = hotkey->modifiers | MOD_SHIFT;
			else if (strcmp(ts->tokens[i], "alt") == 0)
				hotkey->modifiers = hotkey->modifiers | MOD_ALT;
			else if (strcmp(ts->tokens[i], "ctrl") == 0)
				hotkey->modifiers = hotkey->modifiers | MOD_CONTROL;
			else if (strcmp(ts->tokens[i], "win") == 0)
				hotkey->modifiers = hotkey->modifiers | MOD_WIN;
			else
			{
				printf("hotkey_register: %s is not an modifier\n", ts->tokens[i]);
				hotkey_remove(hotkey);
				Tokenize_String_Delete(ts);
				return 1;
			}
		}
	}

	Tokenize_String_Delete(ts);

	if (RegisterHotKey(NULL, hotkey_count + 1 ,  hotkey->modifiers, hotkey->key))
	{
		printf("hotkey_register: registered \"%s\".\n", hotkey_combination);
		hotkey->f = function;
		hotkey_count++;
		return 0;
	}
	else
	{
		hotkey_remove(hotkey);
		printf("hotkey_register: registering \"%s\" failed.\n", hotkey_combination);
		return 1;
	}
}

void HotKey_Call(WORD modifiers, WORD key)
{
	struct HotKey *h;

	h = hotkey_list;

	while (h)
	{
		if (h->modifiers == modifiers)
			if (h->key == key)
				h->f();
		h = h->next;
	}
}



