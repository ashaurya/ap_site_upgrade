#include <stdio.h>
#include <unistd.h>

int main() {
	int c = 10;
	int ff;

	if ((ff=fork()) == 0) {
		sleep(5);
		printf("Child process running, pid = %d, ff = %d, c = %d\n", getpid(), ff, c);
	}

	else {
		sleep(10);
		c = 20;
		printf("Parent process running, pid = %d, ff = %d, c = %d\n", getpid(), ff, c);
	}
	return 0;
}
