
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "rpn.h"

#define MAXCOLS 80
#define MAXROWS 40

char plot[MAXROWS][MAXCOLS];

void clearPlot()
{
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			plot[i][j] = ' ';
		}
	}
}

void printPlot()
{
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			printf("%c", plot[i][j]);
		}
		printf("\n");
	}
}

void plotXY(int x, int y, char c) {
	if ( x <0 || x>=MAXCOLS || y < 0 || y >=MAXROWS) return;
	plot[y][x]=c;
}

void createPlot( char * funcFile, double minX, double maxX) {
	int nvals = MAXCOLS;
	double yy[MAXCOLS];
	double step = (maxX - minX)/(nvals);
	int i, m, k, value;
	double j, temp;
	double num = minX;
	double minY = 10000.0;
	double maxY = -10000.0;

	clearPlot();
	//Evaluate function and store in vector yy
	for (i = 0; i < nvals; i++) {
		yy[i] = rpn_eval(funcFile, num);
		num += step;
	}
	//Compute maximum and minimum y in vector yy
	/*for (i = 0; i < MAXCOLS; i++) {
		for (m = 0; m < MAXCOLS - i - 1; m++) {
			if (yy[m] > yy[m+1]) {
				temp = yy[m];
				yy[m] = yy[m + 1];
				yy[m + 1] = temp;
			}
		}
	}
	minY = yy[0];
	maxY = yy[MAXCOLS];*/

	for (k = 0; k < nvals; k++) {
		if (yy[k] <= minY) {
			minY = yy[k];
		}
		if (yy[k] >= maxY) {
			maxY = yy[k];
		}
	}

	//Plot x axis
	if (minY >= 0.0) {
		for (i = 0; i < nvals; i++) {
			plotXY(i, MAXROWS-1, '_'); 
		}
	}
	else {
		for (i = 0; i < nvals; i++) {
			plotXY(i, MAXROWS / 2 - 1, '_'); 
		}
	}

	//Plot y axis
	/*if (minX < 0.0) {
		for (i = 0; i < nvals; i++) {
			plotXY(MAXCOLS-1, i, '|'); 
		}
	}
	else {
		for (i = 0; i < nvals; i++) {
			plotXY(MAXCOLS / 2 - 1, 2, '_'); 
		}
	}*/
	for (i = 0; i < MAXROWS; i++) {
		plotXY(MAXCOLS / 2 , i , '|'); 
	}

	//Plot function. Use scaling.
	// minX is plotted at column 0 and maxX is plotted ar MAXCOLS-1
	// minY is plotted at row 0 and maxY is plotted at MAXROWS-1
	if (minY >= 0.0) {
		for (i = 0; i < nvals; i++) {
			value = MAXROWS - (int)( yy[i] / step) - 1;
			plotXY(i , value, '*');
		}
	}
	else {
		for (i = 0; i < nvals; i++) {
			value =(MAXROWS / 2 -  (int)((MAXROWS / 2 * (yy[i]))));
			plotXY(i , value, '*');
		}
	}

	printPlot();

}

int main(int argc, char ** argv)
{
	printf("RPN Plotter.\n");
	
	if (argc < 4) {
		printf("Usage: plot func-file xmin xmax\n");
		exit(1);
	}

	// Get arguments
	char * funcName = argv[1];
	double xmin = atof(argv[2]);
	double xmax = atof(argv[3]);
	createPlot(funcName, xmin, xmax);
}
