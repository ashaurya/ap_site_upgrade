/*
 * Access Point simulator
 * Creates cloned processes, with a different random MAC address and connects to server
 * Sends an activity message with MAC address, Version Information
 * Written by Aayush Shaurya
 */

#include "client.h"
#include <sys/random.h>

int client_init(){

	char *hostname = "localhost";
	int sockfd, n;
	unsigned char *buffer;
	struct sockaddr_in serveraddr;
	struct hostent *server;
	socklen_t server_len;

	printf("Generating MAC Address\n");
	MAC = gen_mac();

	ap_boot();
	
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		perror("Error opening socket");
		return -1;
	}

	struct timeval tv;
	tv.tv_sec = 10;
	tv.tv_usec = 0;
	n = setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
	if (n < 0)
		perror("Error in RCVTIMEO");
	n = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
	if (n < 0) perror("Error in SNDTIMEO");
	
	server = gethostbyname(hostname);
	bzero((char *)&serveraddr, sizeof(serveraddr));
  
	serveraddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
	serveraddr.sin_port = htons(PORTNO);
	memset(serveraddr.sin_zero, '\0', sizeof(serveraddr.sin_zero));

	buffer = (unsigned char *)malloc(BUFFER);
	server_len = sizeof(serveraddr);


 AP_CONNECT:
	bzero(buffer, BUFFER);
	bcopy("MAC", buffer, 3);
	bcopy(MAC, buffer+3, 12);
	bcopy("VER", buffer+15, 3);
	bcopy(version, buffer+18, 6);

	n = sendto(sockfd, buffer, strlen(buffer), MSG_NOSIGNAL, (struct sockaddr *)&serveraddr, server_len);
	printf("Sent %d/%d bytes of data to %s\n", n, BUFFER, "127.0.0.1");

	bzero(buffer, BUFFER);
	n = recvfrom(sockfd, buffer, BUFFER, 0, (struct sockaddr *)&serveraddr, &server_len);
	printf("Response: %s", buffer);
	
	char update_message[6];
	bcopy(buffer, update_message, 6);
	if (strcmp("UPDATE", update_message) == 0) {
		printf("\nUpdate message received, goint to AP boot\n");
		ap_boot();
		goto AP_CONNECT;
	}

	if(strcmp("SUCCES", update_message) == 0) {
		time_t rawtime;
		struct tm *time_info;
		time(&rawtime);
		time_info = localtime(&rawtime);
		printf("\nAP running version %s, last update: %s\n", version, asctime(time_info));
		sleep(10);
		goto AP_CONNECT;
	}
	printf("Empty\n");
	goto AP_CONNECT;
	return 0;
}

uint8_t *gen_mac() {
	uint8_t *mac = (uint8_t *)malloc(12);
	getrandom(mac, 12, 0);
	for(int i = 0; i < 12; i++) {
		mac[i] = mac[i]%16 + 100;
		if(i%2 == 1)
			printf("%x:", mac[i]-100);
		else
			printf("%x", mac[i]-100);
	}
	printf("\b \n");
	return mac;
}

void ap_boot() {
	printf("Booting up AP\n");
	fflush(stdout);
	srand(time(NULL));
	uint8_t offset;
	getrandom(&offset, 1, 0);
	offset = offset % 32;
	sleep(104+offset);

	printf("Boot Time: %d (s)\n", 104+offset);

	char buf[6];
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	struct sockaddr_in serveraddr;
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(49433);
	socklen_t server_len = sizeof(serveraddr);
	int n = sendto(sockfd, "Hi", 2, 0, (struct sockaddr *)&serveraddr, server_len);
	n = recvfrom(sockfd, buf, 6, 0, (struct sockaddr *)&serveraddr, &server_len);
	printf("Version: %s\n", buf);
	version = (char *)malloc(6);
	bcopy(buf, version, 6);
}
