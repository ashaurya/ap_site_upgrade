/*
 * Wireless Controller code simulator for AP site upgrade
 * Using UDP for commmunication
 * Written by Aayush Shaurya
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/random.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORTNO 49432
#define BUFFER 65536
#define N_MAX_AP 100
#define SHM_KEY 5467971
#define NEIGHBOR_SHM 5467972

void create_table();
int neighbor_table();

typedef struct node_AP node_AP;
node_AP * make_node();
int shm_id;
key_t shm_key;
node_AP *node_table;

node_AP *create_table_entry(node_AP*);
void site_upgrade();
void *lookup(uint8_t *, uint8_t);

struct node_AP {
	uint8_t MAC_address[16];//1
	char version[6];        //2
	int reboot_lock;        //3
	int reset_required;     //4
};
