
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Separates the file into words
//

#define MAXWORD 200
char word[MAXWORD];
int wordLength;

// It returns the next word from fd.
// If there are no more more words it returns NULL.

char * nextword(FILE * fd) {
	// While it is not EOF read char
	// While it is not EOF and it is a non-space char
	// store character in word.
	// if char is not in word return word so far.
	char ch;
	int i = 0;
	int j;
	size_t length;
	while(1) {
		ch = fgetc(fd);
		for (j = i; j < MAXWORD; j++) {
			word[j] = '\0';
		}
		if ((ch >= 33 && ch <= 127)) {
			word[i] = ch;
			i++;
		}
		else if ( i != 0) {
			//length = strlen(word);
		//}
		//if (i >= length && i >= 1) {
			return word;
		}
		else if (feof(fd)) {
			break; 
		}
		//length = 0;
	}
	/*while((ch = fgetc(fd)) != EOF) {
		for (j = i; j < MAXWORD; j++) {
			word[j] = '\0';
		}
		if ((ch >= 33 && ch <= 127)) {
			word[i] = ch;
			i++;
		}
		if (i != 0){
			return word;
		}
	}*/

		//if (ch == ' ' || ch == '\n' || ch == '\0' || ch == '\t' || ch == '\r') 

	//len = strlen(word);
	//if (ch == EOF) {
		//return word;
	//}
	// Return null if there are no more words
	return NULL;

}

