#define _GNU_SOURCE
#include "client.h"
#include <sched.h>

#define STACK_SIZE 8192

int main(int argc, char **argv) {
	int clients;
	if(argc == 2)
		clients = atoi(argv[1]);
	else
		clients = 1;

	char *stack;
	stack = malloc(STACK_SIZE);
	
	for(int i = 0; i < clients; i++)
		if(fork() == 0)
			client_init();
	wait(NULL);
	return 0;
}
