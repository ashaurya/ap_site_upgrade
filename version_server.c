/* 
 * Version Server
 * Specifies the current version of OS for APs
 * 
 * Written by Aayush Shaurya
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <assert.h>
#include <errno.h>
#include <sched.h>

pid_t parent_pid;
void sigquit_handler(int);
void sigint_handler(int);
void update_ver(char *);

int main(int argc, char **argv) {

	char *VERSION = (char *)malloc(6);
	parent_pid = getpid();
	signal(SIGQUIT, sigquit_handler);
	signal(SIGINT, sigint_handler);
	if(argc == 2)
		bcopy(argv[1], VERSION, 6);
	else
		bcopy("16.5.7", VERSION, 6);
	
	int sockfd;
	int clientlen;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	char buf[1];
	int optval;
	int n;

 VERSION_SERVER:
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		perror("Error opening socket");
		sleep(2);
		goto VERSION_SERVER;
	}

	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));
	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(tv));

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(49433);

	n = bind(sockfd, (struct sockaddr *) &serveraddr, sizeof(serveraddr));
	if(n < 0) {
		perror("Error in bind");
		sleep(2);
		goto VERSION_SERVER;
	}

 HANDLE_CLIENTS:
	clientlen = sizeof(clientaddr);
	
	printf("Starting server, version %s, PID %d\n", VERSION, getpid());

	if(fork() == 0) {
		printf("Listening... PID %d\n", getpid());
		while (1) {
			n = recvfrom(sockfd, buf, 0, 0, (struct sockaddr *) &clientaddr, &clientlen);
			n = sendto(sockfd, VERSION, 6, 0, (struct sockaddr *)&clientaddr, clientlen);
			printf("Successfully sent version: %s\n", VERSION);
		}
	}
	else {
		char buffer[3];
		scanf("%s", &buffer);
		if( strcmp(buffer, "END") == 0)
			return 0;
		fflush(stdin);
		printf("Restarting server...\n");
		kill(-parent_pid, SIGQUIT);
		update_ver(VERSION);
		goto HANDLE_CLIENTS;
	}
	return 0;
}

void sigquit_handler(int sig) {
	assert(sig == SIGQUIT);
	pid_t self = getpid();
	if (parent_pid != self)
		_exit(0);
}

void sigint_handler(int sig) {
	assert(sig == SIGINT);
}

void update_ver(char *ver) {
	if(ver[5] < '9')
		ver[5]++;
	else if(ver[3] < '9') {
		ver[3]++;
		ver[5] = '0';
	}
	else if(ver[1] < '9') {
		ver[1]++;
		ver[3] = ver[5] = '0';
	}
	else {
		ver[0]++;
		ver[1] = ver[3] = ver[5] = '0';
	}
	return;
}
