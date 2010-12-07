struct rule
{
	char *name;
	struct regexp *name_regexp;
	struct regexp *class_regexp;
	struct section **s;
	int section_count;
	struct rule *next, *prev;
};

struct rule *Rule_Add(char *name, char *name_regexp, char *class_regexp);
int Rule_Add_Section(struct rule *r, struct section *s);
int Rule_Check_Window(struct window *w);
