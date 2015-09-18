
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct WordInfo {
	char * word;
	int count;
};

typedef struct WordInfo WordInfo;

int maxWords = 100000;
int nWords = 0;
WordInfo*  wordArray;

#define MAXWORD 100
int wordLength = 0;
char word[MAXWORD];
FILE * fd;
int charCount;
int wordPos;


void toLower(char *s) {
	int i;
	char ch;
	int length = strlen(s);
	for (i = 0; i < length; i++ ) {
		ch = s[i];
		if (ch >= 'A' && ch <= 'Z') {
			ch = (ch - 'A') + 'a';
		 	s[i] = ch;
		}
	}
}


static char * nextword() {
	char ch;
	int i = 0;
	int j;
	charCount = 0;
	while( (ch = fgetc(fd)) != EOF) {
		charCount++;
		for (j = i; j < MAXWORD; j++) {
			word[j] = '\0';
		}
		if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
			word[i] = ch;
			wordLength++;
			i++;
		}
		else if (i != 0) {
			//wordPos = charCount - wordLength - 1;
			i = 0;
			return word;
		}
	}
	return NULL;
}



int main(int argc, char **argv) {

	if (argc < 2) {
		printf("Usage: countwords filename\n");
		exit(1);
	}

	char * filename = argv[1];
	fd = fopen(filename, "r");
	if (fd == NULL) {
		printf("Error!");
		exit(1);
	}

	char ch;
	int i = 0;
	int j = 0;
	int found;
	char * word;
	int cmp = 5;
	int count = 0;
	int totalWords = 0;
	wordArray = (WordInfo *) malloc(maxWords * sizeof(WordInfo));

	while((word = nextword(fd)) != NULL) {
		totalWords++;
		toLower(word);
		found = 0;
		for (j = 0; j < nWords; j++) {
			cmp = strcmp(wordArray[j].word, word);
			if (cmp == 0) {
				wordArray[j].count++;
				found = 1;
				break;
			}
		}
		if (found != 1) {
			wordArray[nWords].word = strdup(word);
			wordArray[nWords].count++;
			nWords++;
		}
	}


	//bubble sort
	char * temp;
	int tempCount; 

	for(i = 0; i < nWords; i++) {
		for(j = 0; j < nWords; j++) {
			if (strcmp(wordArray[i].word, wordArray[j].word) < 0 ) {
				temp = strdup(wordArray[j].word);
				wordArray[j].word = strdup(wordArray[i].word);
				wordArray[i].word = temp;

				tempCount = wordArray[j].count;
				wordArray[j].count =wordArray[i].count;
				wordArray[i].count = tempCount;
			}
		}
	}

	for (i = 0; i < nWords; i++) {
		printf("%s %d\n", wordArray[i].word, wordArray[i].count);
	}
}