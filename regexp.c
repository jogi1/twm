#include <stdio.h>
#include <string.h>
#include "pcre.h"

char *Regexp_Get_Substring_From_Buffer(pcre *regexp, char *buffer, int substring)
{
	char *file;
	int rc, ovector[30];
	char *viewstate;

	if (regexp == NULL)
		return NULL;

	if (buffer == NULL)
		return NULL;

	rc = pcre_exec(regexp, NULL, buffer, strlen(file), 0, 0, ovector, 30);
	if (rc != -1)
	{
		rc = pcre_get_substring(buffer, ovector, 30, substring, (const char **)&viewstate);
		if (rc != -1)
		{
			return viewstate;
		}
		else
			printf("Get_Regexp_From_File: could not get substring.\n");
	}
	else
		printf("Get_Regexp_From_File: regexp didnt match.\n");

	return NULL;
}

struct regexp *Regexp_Compile(char *string)
{
	pcre *regexp;
	const char *error;
	int erroroffset;

	regexp = pcre_compile(string, 0, &error, &erroroffset, NULL);
	
	if (regexp == NULL)
	{
		printf("error: \"%s\"\n", error);
		printf("%s\n", string);
		printf("%*s^", erroroffset, "^");
		return NULL;
	}

	return (struct regexp *)regexp;
}

int Regexp_Match_String(struct regexp *regexp, char *string)
{
	pcre *r;
	int ovector[2];

	if (regexp == NULL || string == NULL)
		return -1;

	r = (pcre *)regexp;

	return pcre_exec((pcre *)r, NULL, string, strlen(string), 0 ,0, ovector, 2);
}


