struct regexp { void *v;};
struct regexp *Regexp_Compile(char *string);
int Regexp_Match_String(struct regexp *regexp, char *string);
