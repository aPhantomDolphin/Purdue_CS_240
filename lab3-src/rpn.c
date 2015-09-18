
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>

#include "rpn.h"
#include "nextword.h"
#include "stack.h"

double rpn_eval(char * fileName, double x) {
	double a, b, c;
	FILE *fd = fopen(fileName, "r");
	char * word;
	char ch;
	int countW = 0;
	int countO = 0;
	double result;


	while((word = nextword(fd)) != NULL) {
		if (strcmp("+", word) == 0) {
			a = stack_pop();
			b = stack_pop();
			stack_push(b + a);
			countO++;
		}
		else if (strcmp("-", word) == 0) {
			a = stack_pop();
			b = stack_pop();
			stack_push(b - a);
			countO++;
		}
		else if (strcmp("*", word) == 0) {
			a = stack_pop();
			b = stack_pop();
			stack_push(b * a);
			countO++;
		}
		else if (strcmp("/", word) == 0) {
			a = stack_pop();
			b = stack_pop();
			stack_push(b / a);
			countO++;
		}
		else if (strcmp("sin", word) == 0) {
			a = stack_pop();
			stack_push(sin(a));
			//countO++;
		}
		else if (strcmp("cos", word) == 0) {
			a = stack_pop();
			stack_push(cos(a));
			//countO++;
		}
		else if (strcmp("pow", word) == 0) {
			a = stack_pop();
			b = stack_pop();
			stack_push(pow(b, a));
			countO++;
		}
		else if (strcmp("log", word) == 0) {
			a = stack_pop();
			stack_push(log(a));
			//countO++;
		}
		else if (strcmp("exp", word) == 0) {
			a = stack_pop();
			stack_push(exp(a));
			//countO++;
		}
		else if (strcmp("x", word) == 0) {
			stack_push(x);
			countW++;
		}
		else {
			c = atof(word);
			stack_push(c);
			countW++;
		}
	}
	/*result =  stack_pop();
	if (countW < countO || countO == 0) {
		//printf("The countO : %d ; The countW : %d\n", countO, countW);
		printf("Elements remain in the stack\n");
		exit(1);
	}
	else if (stack_is_empty() == 0) {
		//printf("stack_top: %d\n", stack_top());
		//printf("The remaining : %f\n", stack_pop());
		//printf("The countO : %d ; The countW : %d\n", countO, countW);
		printf("Stack underflow\n");
		exit(1);
	}
	else {
		return result;
	}*/
	if (countW - countO > 1) {
		printf("Elements remain in the stack\n");
		exit(1);
	}
	else if (countW - countO < 1) {
		printf("Stack underflow\n");
		exit(1);
	}
	else {
		result = stack_pop();
		return result;
	}
}

