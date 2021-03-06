/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU

    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
			Guy Vega, gtvega@seabass.st.usm.edu
			Dominic Tynes, dbtynes@seabass.st.usm.edu

    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
			Peng Piaw Foong, ppfoong@csie.ncu.edu.tw

    Firebird2000 Bulletin Board System
    Copyright (C) 1999-2001, deardragon, dragon@fb2000.dhs.org

    Puke Bulletin Board System
    Copyright (C) 2001-2002, Yu Chen, monster@marco.zsu.edu.cn
			     Bin Jie Lee, is99lbj@student.zsu.edu.cn

    Contains codes from YTHT & SMTH distributions of Firebird Bulletin
    Board System.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

#include "webbs.h"

#ifdef FILTER

#include <regex.h>
//#include "edit.h"

#define MAX_FILTERS 16


regex_t preg[MAX_FILTERS];
int filter_count = -1;

void
init_filter()
{
	FILE *fp;
	struct stat st;
	char buf[256];

	filter_count = 0;

	if ((fp = fopen("etc/filter_words", "r")) == NULL) {
		return;
	}

	if (fstat(fileno(fp), &st) == -1 || st.st_size <= 0) {
		fclose(fp);
		return;
	}

	while (filter_count < MAX_FILTERS && fgets(buf, 256, fp) != NULL) {
		int len = strlen(buf);

		if (len >= 2) {
			if (buf[len - 1] == '\n') {
				buf[len - 1] = 0;
			}

			if (regcomp(&preg[filter_count], buf,  REG_EXTENDED | REG_ICASE | REG_NEWLINE) == 0) {
				filter_count++;
			}
		}
	}

	fclose(fp);
}

int
regex_strstr(const char *haystack)
{
	int i;
	regmatch_t match;

	for (i = 0; i < filter_count; i++) {
		if (regexec(&preg[i], haystack, 1, &match, 0) == 0)
			return 0;
	}
	return -1;
}

int
str_gets(char *text, char *line, int size){
	int i, length;
	
	if(strlen(text) > size)
		length = size;
	else
		length = strlen(text);

	for(i = 0; i < length; i++){
		*(line + i) = *(text + i);
		if(*(text + i) == '\n'){
			*(line + i + 1) = '\0';
			return i + 1;
		}
	}
	*(line + i) = '\0';
	return i;
}

int
check_text(char *title, char *content)
{
	char line[1024];
	char *ptr = content;
	int len;
	
	if (filter_count == -1) {
		init_filter();
	}

	if (regex_strstr(title) == 0)
		return 0;
	
	while ( (len = str_gets(ptr, line, sizeof(line))) > 0) {
		if (regex_strstr(line) == 0)
			return 0;
		ptr += len;
	}
	return -1;
}
#endif
