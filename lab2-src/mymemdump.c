	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>


void mymemdump(FILE * fd, char * p , int len) {

	//declare needed variables
	int i;
	int j;
	int k;
	unsigned long q = p;
	int n;

	char sp = ' ';
	int rest = len % 16;
	int ln = len / 16;


	
	fprintf(fd, "0x%016lX: ", (unsigned long) p);
	for (i = 0; i < len - 1; i++) {
		int c = p[i]&0xFF; 
		fprintf(fd, "%02X ", c);
		j = i + 1;
		if (j % 16 == 0 && ln >= 1) {
			fprintf(fd, "%1c", sp);
			for (k = 0; k < 16 && j < 17; k++) {
				int c = p[k]&0xFF; 
				if (c != EOF)
					fprintf(fd, "%-c", (c >= 32 && c <= 127)?c:'.');
			}
			for (k = j - 16; k < j && j >= 17; k++) {
				int c = p[k]&0xFF; 
				if (c != EOF)
					fprintf(fd, "%c", (c >= 32 && c <= 127)?c:'.');
			}
			fprintf(fd,"\n");
			q += 16;
			fprintf(fd, "0x%016lX: ", (unsigned long) q); 
		}
	}
	int c = p[len - 1]&0xFF; 
	fprintf(fd, "%02X ", c);
	fprintf(fd, "%1c", sp);
	if (rest == 0) {
		for (n = len - 16; n < len; n++) {
			int c = p[n]&0xFF; 
			fprintf(fd, "%c", (c >= 32 && c <= 127)?c:'.');
		}
	}
	else {
		for (n = 0; n < 16 - rest; n++) {
			fprintf(fd, "%3c", sp);
		}
		for (n = len - rest; n < len; n++) {
			int c = p[n]&0xFF; 
			fprintf(fd, "%c", (c >= 32 && c <= 127)?c:'.');
		}
	}
	fprintf(fd,"\n");
}