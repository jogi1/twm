struct rule
{
	char *name;
	struct regexp *name_regexp;
	struct regexp *class_regexp;
	struct section *s;
	struct rule *next, *prev;
};

int Rule_Add(char *name, struct section *s, char *name_regexp, char *class_regexp);
int Rule_Check_Window(struct window *w);
