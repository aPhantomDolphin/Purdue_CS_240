
#include <stdlib.h>
#include "mystring.h"

int mystrlen(char * s) {
	int len = 0;
	char *p = s;
	while (*p != '\0') {
		len++;
		p++;
	}
	return len;
}

char * mystrcpy(char * dest, char * src) {
	char *p = src;
	char *q = dest;
	while (*p != '\0') {
		*q = *p;
		q++;
		p++;
	}
	*q = '\0';
	return dest;
}

char * mystrcat(char * dest, char * src) {
	char *p, *q;
	p = dest;
	while (*p) {
		p++;
	}
	q = src;
	while (*q) {
		*p = *q;
		p++;
		q++;
	}
	*p = '\0';
	return dest;
}

int mystrcmp(char * s1, char * s2) {
	int len1 = mystrlen(s1);
	int len2 = mystrlen(s2);
	char *p = s1;
	char *q = s2;
	int ans = 0;
	if (len1 < len2) 
		return -1;
	else if (len1 > len2) 
		return 1;
	else {
		while(*p) {
			if (*p == *q) {
				p++;
				q++;
			}
			else 
				return -1;
		}
		return 0;
	}
}

char * mystrstr(char * hay, char * needle) {
	int lenn = mystrlen(needle);  
	int lenh = mystrlen(hay);     
	int length = lenh - lenn;     

	int i = 0;
	int count = 0;
	char * needlep;
	char * hayp;

	for (i = 0; i < length; i++) {
		needlep = needle;
		hayp = hay;
		count = 0;
		while (needlep != '\0') {  // as long as there 
								   //still entries in needle
			if (*needlep == *hayp) {  
				needlep++;
				hayp++;
				count++;
				if (count == lenn) {
					return hay;
				}
				else ;
			}
			else {
				break;
			}
		}
		hay++;
	}

	return NULL;
}

char * mystrdup(char * s) {
	int len = mystrlen(s);
	char *cp = (char *)(malloc (len));
	if (cp == NULL) 
		return NULL;
	else 
		mystrcpy(cp,s);
	return cp;
}

char * mymemcpy(char * dest, char * src, int n)
{
	int i;
	for (i = 0; i < n;i++) {
		*dest++ = *src++;
	}
	return dest; 
}

