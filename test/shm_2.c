#define SHM_KEY 12312312

#include <sys/shm.h>
#include <sys/ipc.h>

#include <string.h>
#include <stdio.h>

int main() {
	int shm_id = shmget(SHM_KEY, 1000, IPC_CREAT | 0666);
	char *buffer = (char *)shmat(shm_id, 0,0);
	printf("%s\n", buffer);
	return 0;
}
