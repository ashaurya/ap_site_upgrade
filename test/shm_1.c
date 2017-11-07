#define SHM_KEY 12312312

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main() {
	int shm_id = shmget(SHM_KEY, 1000, IPC_CREAT | 0666);
	char *buffer;
	buffer = (char *)shmat(shm_id, 0, 0);
	bcopy("Hi, this is a test segment", buffer, 26);
	while(1) {
		char str[265];
		scanf("%s", str);
		bcopy(str, buffer, sizeof(str));
		
	}
	return 0;
}
