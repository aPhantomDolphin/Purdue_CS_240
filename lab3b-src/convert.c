#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int baseTo (char* num, int base) {
	int ans = 0;
	int length = strlen(num);
	int i, k;
	for (i = 0; i < length; i++) {
		k = length - i - 1;
		if (num[i] >= 65 && num[i] <= 90)
			ans += ((num[i] - 55) * pow(base, k));
		else if (num[i] >= 48 && num[i] <= 57)
			ans += ((num[i] - 48) * pow(base, k));
	}
	return ans;
}

int baseFrom (int num, int base) {
	int result[100];
	char poss[36] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
	/*char poss[100];
	int m;
	int n = 0;
	for (m = 48; (m >= 48 && m <= 57) || (m <= 65 && m <= 90); m++) {
		poss[n] = (char)m;
		n++;
	}*/
	int i = 0;
	int ans;
	while (num != 0 || num != '\0') {
		result[i] = num % base;
		num = num / base;
		i++;
	}
	printf("Converted to base %d: ", base );

	int j = i - 1;
	while(j >= 0) {
		printf("%c", poss[result[j]]);
		ans = poss[result[j]];
		j--;
	}
	printf("\n");
	return ans;
}

main(int argc, char** argv) {
	if (argc < 4)
	{
		printf("Usage:  convert <basefrom> <baseto> <number>\n");
		exit(1);
	}
	int fromBaseNum, toBaseNum, i;
	char ch;
	fromBaseNum = atoi(argv[1]);
	toBaseNum = atoi(argv[2]);
	int numLength = strlen(argv[3]);

	printf("Number read in base %d: %s\n", fromBaseNum, argv[3]);

	while(numLength >= 0) {
		int base;
		ch = argv[3][numLength - 1];
		//ch = getchar();
		if (ch >= 48 && ch <= 57)
			base = ch - 48;
		else if (ch >= 65 && ch <= 90)
			base = ch - 55;
		if (base > fromBaseNum) {
			printf("Wrong digit in number.\n");
			exit(1);
		}
		numLength--;
	}

	//int toBase = atof(argv[3]);
	int num = baseTo (argv[3], fromBaseNum);
	int ans1 = baseFrom (num, 10);
	int ans2 = baseFrom (num, toBaseNum);
}
