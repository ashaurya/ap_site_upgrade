#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char *update();

int main() {
	char *test = (char *)malloc(6);
	bcopy("18.7.7", test, 6);
	char ch;
	while ((ch = getchar()) == '\n')
		printf("%s\n", update(test));
	return 0;
}

char *update(char *test) {
	if(test[5] < '9') {
		test[5]++;
		return test;
	}
	if(test[3] < '9') {
		test[3]++;
		test[5] = '0';
		return test;
	}
	if(test[1] < '9') {
		test[1]++;
		test[3] = '0';
		test[5] = '0';
		return test;
	}
	test[0]++;
	test[1] = test[3] = test[5] = '0';
	return test;
}
