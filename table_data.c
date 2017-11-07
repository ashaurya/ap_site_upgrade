#include <sys/shm.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>


#define SHM_KEY 5467971
#define MAX_AP 100
#define NEIGHBOR_SHM 5467972

int neighbor_table();

typedef struct node_AP {
	uint8_t MAC_address[16];
	char version[6];
	int reboot_lock;
	int reset_required;
} node_AP;

int main() {
	int shm_id = shmget(SHM_KEY, MAX_AP*sizeof(node_AP), 0666);
	node_AP *node_table;
	node_table = (node_AP *)shmat(shm_id, 0, 0);

	for(int i = 0; i < 100; i++) {
		printf("MAC: ");
		for(int j = 0; j < 12; j++) {
			if(j%2 == 1)
				printf("%x:", node_table[i].MAC_address[j]);
			else
				printf("%x", node_table[i].MAC_address[j]);
		}
		printf("\b  Version: %s\n", node_table[i].version);
	}

	//neighbor_table();
	return 0;
}

int neighbor_table() {
	int shm_id = shmget(NEIGHBOR_SHM, 100*100*sizeof(int8_t), 0666);
	int8_t (*adj_matrix)[100] = shmat(shm_id, 0, 0);
	
	for(int i = 0; i < 100; i++) {
		for(int j = 0; j < 100; j++) {
			printf("%d ", adj_matrix[i][j]);
		}
		printf("\n");
	}
	return 0;
}
