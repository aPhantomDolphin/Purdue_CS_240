
#include <stdlib.h>
#include "mystring.h"

// Type "man string" to see what every function expects.

int mystrlen(char * s) {
	int i = 0;
	while (s[i] != '\0') {
		i++;
	}
	return i;
}

char * mystrcpy(char * dest, char * src) {
	int i = 0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return dest;
}

char * mystrcat(char * dest, char * src) {
	int i;
	int len = mystrlen(dest);
	for (i = 0; i < mystrlen(src); i++)
		dest[len + i] = src[i];
	dest[len + i] = '\0';
	return dest;
}

int mystrcmp(char * s1, char * s2) {
	int i;
	if (mystrlen(s1) == mystrlen(s2)) {
		for (i = 0; i < mystrlen(s1); i++) {
			if (s1[i] == s2[i])
				return 0;
		}
	}
	else if (mystrlen(s1) > mystrlen(s2)) {
		return 1;
	}
	else 
		return -1;
}

char * mystrstr(char * hay, char * needle) {
	int lenn = mystrlen(needle);  //length of needle
	int lenh = mystrlen(hay);     //length of hay
	int length = lenh - lenn;     //the length that hay is 
								  //longer than needle

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

