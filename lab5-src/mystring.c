
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mystrlen(char * s) {
	int i = 0;
	while (s[i] != '\0') {
		i++;
	}
	return i;
}


// Adds src at the end of dest. Return the value of dest.
char *mystrcat(char * dest, char * src) {
	int i;
	int len = mystrlen(dest);
	for (i = 0; i < mystrlen(src); i++)
		dest[len + i] = src[i];
	dest[len + i] = '\0';
	return dest;
}


// Return a pointer to the first occurrence of the substring needle in
// the string haystack. 
// Return NULL if the string is not found.
char * mystrstr(char * haystack, char * needle) {
	int lenn = mystrlen(needle);  //length of needle
	int lenh = mystrlen(haystack);     //length of hay
	int length = lenh - lenn;     //the length that hay is 
								  //longer than needle

	int i = 0;
	int count = 0;
	char * needlep;
	char * hayp;

	for (i = 0; i < length; i++) {
		needlep = needle;
		hayp = haystack;
		count = 0;
		while (needlep != '\0') {  // as long as there 
								   //still entries in needle
			if (*needlep == *hayp) {  
				needlep++;
				hayp++;
				count++;
				if (count == lenn) {
					return haystack;
				}
				else ;
			}
			else {
				break;
			}
		}
		haystack++;
	}

	return NULL;
}


// Trims any spaces that are at the beginning or the end of s and returns s.
// Use the same memory passed in s. 
// Example:
// char s1[20];
// strcpy(s1,      hello    );
// strtrim(s1);
// printf(<s1=%s>\n, s1); // it will print <s1=hello>  without the spaces.
char * mystrtrim(char * s) {
	char *p = s;
	while(*s != '\0') {
		if (*s != ' ') {
			//p++;
			*p++ = *s;
		}
		s++;
	}
	*p = '\0';
	return s;
  }


// Returns a new string that will substitute the first occurrence of the substring from
//  to the substring to in the string src
// This call will allocate memory for the new string and it will return the new string.
// Example:
// char a[6];
// strcpy(a, apple);
// char * b = strreplace(a, pp, orange);
// printf(<b=%s>\n, b); // It will print <b=aorangele>  
char * mystrreplace( char * src, char * from, char * to)  {
	/*int lenfrom = strlen(from);
	int lento = strlen(to);
	int lensrc = strlen(src);

	int length = lensrc + lenfrom + lento;
	char * ans = (char *)malloc(length * sizeof(char));

	char * beg = src;
	char * start;

	int count = 0;
	for (count = 0; start = strstr(beg, from); count++) {
		beg = start + lenfrom;
	}

	while(1) {
		ins = strstr(src, from);
		lenfrom = ins - src;
		start = strncpy(temp, src, lenfrom) + lenfrom;
		start = strcpy(temp, to) + lento;
		src += lenfrom + lenfrom;
	}*/

    char *beg = src;
    char *ans, *temp; 
    int length, count;
    int lento = strlen(to); 
    int lenfrom = strlen(from); 

    for (count = 0; temp = strstr(beg, from); count++) {
        beg = temp + lenfrom;
    }

    temp = ans = malloc(strlen(src) + (lento - lenfrom) * count + 1);

    while (count--) {
        beg = strstr(src, from);
        length = beg - src;
        temp = strncpy(temp, src, length) + length;
        temp = strcpy(temp, to) + lento;
        src += (length + lenfrom);
    }
    strcpy(temp, src);

    return ans;
}


