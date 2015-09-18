#include <stdio.h>

int
main(int argc, char **argv) {
  printf("Welcome to the High Low game...\n");
while(1) {
  printf("Think of a number between 1 and 100 and press press <enter>");
  getchar();
  int low = 1;
  int high = 100;
  int mid = (high + low) / 2;
 char ans;
while(high >= low) {
  printf("Is it higher than %d? (y/n)\n", mid);
  scanf("%c", &ans);
  getchar();
  if (ans == 'y') {
		low = mid + 1;
		mid = (high + low) / 2;
}
  else if (ans == 'n') {
		high = mid - 1;
		mid = (high + low) /2;
}
  else {
		printf("Type y or n\n");
}
}
printf("\n>>>>>> The number is %d\n\n", mid + 1);
printf("Do you want to continue playing (y/n)?");
char answer;
answer = getchar();
getchar();
if (answer == 'n') {
		break;
}
}
printf("Thanks for playing!!!\n");
  // Write your implementation here...

}

