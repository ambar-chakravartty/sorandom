#include <stdio.h>
#define RAND_MAX ((1U << 31) - 1)
/* Implementation of a LCG */

int rseed = 0;

void srand(int x) {
  rseed = x;
}
	   
int rand() {
  return rseed = (rseed * 1103515245 + 12345) & RAND_MAX;
}

int main() {
  printf("RAND_MAX is %d\n", RAND_MAX);
  srand(2);
  for (int i = 0; i < 10; i++) {
    printf("%d\n", rand());
  }
  return 0;
}
