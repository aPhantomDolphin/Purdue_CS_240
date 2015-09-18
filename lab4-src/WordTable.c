
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "WordTable.h"

// Initializes a word table
void wtable_init(WordTable * wtable)
{
	// Allocate and initialize space for the table
	//wtable = (WordTable *) malloc(sizeof(WordTable));
	wtable->nWords = 0;
	wtable->maxWords = 10;
	wtable->wordArray = (WordInfo *) malloc(wtable->maxWords * sizeof(WordInfo));

	for (int i = 0; i < wtable->maxWords; i++) {
		llist_init(&wtable->wordArray[i].positions);
	}
}

// Add word to the tableand position. Position is added to the corresponding linked list.
void wtable_add(WordTable * wtable, char * word, int position)
{
	//Find first word if it exists
	int i = 0;
	for (i = 0; i < wtable->nWords; i++) {
		if ( strcmp(wtable->wordArray[i].word, word) == 0 ) {
			// Found word. Add position in the list of positions
			llist_insert_last(&wtable->wordArray[i].positions, position);
			return;
		}
	}

	if (wtable->nWords == wtable->maxWords) {
		wtable->maxWords = (wtable->maxWords) * 2;
		wtable->wordArray = (WordInfo *) realloc(wtable->wordArray, (wtable->maxWords) * sizeof(WordInfo));
		
	}
	// Word not found.
	// Make sure that the array has space.
	// Expand the wordArray here.

	// Add new word and position
	wtable->wordArray[wtable->nWords].word = strdup(word);
	llist_insert_last(&wtable->wordArray[wtable->nWords].positions, position);
	wtable->nWords++;
	
}

// Print contents of the table.
void wtable_print(WordTable * wtable, FILE * fd)
{
	fprintf(fd, "------- WORD TABLE -------\n");

	// Print words

	for (int i = 0; i <= wtable->nWords; i++) {
		if (wtable->wordArray[i].word != NULL) {
			fprintf(fd, "%d: %s: ", i , wtable->wordArray[i].word);
			llist_print( &wtable->wordArray[i].positions);
		}
		
	}
}

// Get positions where the word occurs
LinkedList * wtable_getPositions(WordTable * wtable, char * word)
{
	// Write your code here
	return NULL;
}

//
// Separates the string into words
//

#define MAXWORD 200
#define isALetter(ch) ((ch>='A' && ch <= 'Z') || (ch >='a' && ch <='z'))
char word[MAXWORD];
int wordCount;
int charCount = 0;
int pos = 0;
// It returns the next word from stdin.
// If there are no more more words it returns NULL.
// A word is a sequence of alphabetical characters.
static char * nextword(FILE * fd) {
	// Write your code here
  	char ch;
  	int i;
  	int j = 0;
  	int wordLength = 0;
	while ( (ch = fgetc(fd)) != EOF ) {
		charCount ++;
		for ( i = j; i < 20; i++)
			word[i] = '\0';
		if (isALetter(ch) && ch != '\n' && ch != ' ' && ch != '\t' ) {
			word[j] = ch;
			wordLength++;
			j++;
		}
		else if ( j != 0){
			pos = charCount - wordLength - 1;
			j = 0;
			return word;
		}
	}
	return NULL;
}

// Conver string to lower case
void toLower(char *s) {
	// Write your code here
	int i;
	char ch;
	for ( i = 0; i < strlen(s); i++ ) {
		ch = s[i];
		if (ch >= 'A'&& ch <= 'Z')
			s[i] = (ch - 65) + 97;
	}
}


// Read a file and obtain words and positions of the words and save them in table.
int wtable_createFromFile(WordTable * wtable, char * fileName, int verbose)
{
	// Write your code here
	FILE * fd = fopen(fileName, "r");
	char * word;
	int i = 0;
	if (fd == NULL) {
		return 0;
	}
	while ( (word = nextword(fd)) != NULL) {
		toLower(word);
		if (verbose == 1) {
			printf("%d: word=%s, pos=%d\n",i, word, pos);
		}
		wtable_add(wtable, word, pos);
		i++;
	}
	return 0;
}

// Sort table in alphabetical order.
void wtable_sort(WordTable * wtable) {
	// Write your code here
	int i;
	int j;
	char * temp;
	LinkedList pos;
	for ( i = 0; i < wtable->nWords; i++) {
		for (j = 0; j < wtable->nWords - 1; j++) {
			if ((strcmp(wtable->wordArray[j].word, wtable->wordArray[j + 1].word)) > 0) {
				temp = wtable->wordArray[j].word;
				pos = wtable->wordArray[j].positions;
				wtable->wordArray[j].word = wtable->wordArray[j + 1].word;
				wtable->wordArray[j + 1].word = temp;
				wtable->wordArray[j].positions = wtable->wordArray[j + 1].positions;
				wtable->wordArray[j + 1].positions = pos;

			}
		}
	}
}

// Print all segments of text in fileName that contain word.
// at most 200 character. Use fseek to position file pointer.
// Type "man fseek" for more info. 
int wtable_textSegments(WordTable * wtable, char * word, char * fileName) {
	// Write your code here
	FILE * fd = fopen(fileName, "r");
	int i, j, k, element;
	long int value;
	char ch;
	char words[MAXWORD];
	printf("===== Segments for word \"%s\" in book \"%s\" =====\n", word, fileName);
	for (i = 0; i < wtable->nWords ; i++) {
		if (strcmp(word, wtable->wordArray[i].word) == 0) {
			element = llist_number_elements(&wtable->wordArray[i].positions);
			for ( j = 0; j < element; j++) {
				llist_get_ith(&wtable->wordArray[i].positions, j , &value);
				k = 0;
				fseek(fd, value, SEEK_SET);
				while (k < MAXWORD) {
					ch = fgetc(fd);
					words[k] = ch;
					k++;
				}
				printf("---------- pos=%d-----\n", value);
				printf("......%s......\n", words);
			}
		}
	}
}

