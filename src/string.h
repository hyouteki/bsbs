#ifndef BSBS_STRING_H_
#define BSBS_STRING_H_

#include <string.h>

static int Bsbs_IsBlankChar(char);
static int Bsbs_IsSpecialChar(char);

void Bsbs_Trim(char **);
char *Bsbs_FetchUntil(char **, char);
void Bsbs_RemoveSpecial(char **);
int Bsbs_StartsWith(char *, char *);
void Bsbs_RemoveTrailing(char **, char);

static int Bsbs_IsBlankChar(char ch) {
	switch (ch) {
	case '\t':
	case '\n':
	case '\r':
	case ' ':
		return 1;
	default:
		return 0;
	}
}

static int Bsbs_IsSpecialChar(char ch) {
	if (Bsbs_IsBlankChar(ch)) return 1;
	switch (ch) {
	case '=':
		return 1;
	default:
		return 0;
	}
}

void Bsbs_Trim(char **text) {
	char *start = *text;
	while (Bsbs_IsBlankChar(*start)) start++;
	char *end = *text + strlen(*text)-1;
    while (end > *text && Bsbs_IsBlankChar(*end)) end--;
    *(end+1) = 0;
	*text = start;
}

char *Bsbs_FetchUntil(char **text, char delim) {
	char *start = *text;
	unsigned int i = 0;
	while (*start != delim && i < strlen(*text)) {
		start++;
		i++;
	}
	char *res = strndup(*text, i);
	*text = start;
	return res;
}

void Bsbs_RemoveSpecial(char **text) {
	char *start = *text;
	while (Bsbs_IsSpecialChar(*start)) start++;
	*text = start;
}

int Bsbs_StartsWith(char *text, char *prefix) {
	if (strlen(text) < strlen(prefix)) return 0;
	for (unsigned int i = 0; i < strlen(prefix); ++i)
		if (text[i] != prefix[i]) return 0;
	return 1;
}

void Bsbs_RemoveTrailing(char **text, char delim) {
	char *end = *text + strlen(*text)-1;
    while (end > *text && *end == delim) end--;
    *(end+1) = 0;
}

#endif // BSBS_STRING_H_
