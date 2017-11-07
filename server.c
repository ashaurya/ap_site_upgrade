/*
 * Wireless Controller code simulator for AP site upgrade
 * Using UDP for commmunication
 *
 * Written by Aayush Shaurya
 *
 */

#include "server.h"

uint8_t _entries;
uint8_t site_upgrade_counter;

int main(int argc, char **argv) {

	_entries = 0;
	create_table();
	neighbor_table();
	
	int sockfd, n;
	unsigned char *buffer;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	socklen_t server_len = sizeof(serveraddr);
	socklen_t client_len = sizeof(clientaddr);
	struct hostent *hostp;
	char *hostaddrp;
	char command_buf[16];

	if(argc != 1 )
		printf("Arguments ignored\n");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if( sockfd < 0 ) {
		perror("Error opening socket");
		return -1;
	}

	printf("Server started on Port no %d, PID %d\n", PORTNO, getpid());


	//Socket Option Section
	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	int broadcast = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(int));

	int keepalive = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(int));

	//End of Socket Options
	
	bzero((char *)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(PORTNO);

	n = bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if( n < 0 ) {
		perror("Error in bind");
		return -1;
	}

	pid_t ff;
	if((ff=fork()) == 0) {
		buffer = (unsigned char *) malloc(BUFFER);
		
		while (1) {
			bzero(buffer, BUFFER);
			n = recvfrom(sockfd, buffer, BUFFER, 0, (struct sockaddr *)&clientaddr, &client_len);
			
			
			if ( n < 0 )
				continue;
			
			hostaddrp = (char *)inet_ntoa(clientaddr.sin_addr);

			printf("AP connected. Getting information...\n");

			uint8_t MAC[16];
			bcopy(buffer+3, MAC, 16);
			char *version = (char *)malloc(6);
			bcopy(buffer+19, version, 6);
			//printf("received: %d, %s\n",n, buffer);
			for(int i = 0; i < 12; i++)
				MAC[i] = MAC[i] - 100;
			printf("MAC: %x%x:%x%x:%x%x:%x%x:%x%x:%x%x\n",
			       MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5], MAC[6],
			       MAC[7], MAC[8], MAC[9], MAC[10], MAC[11]);
			printf("Version: %s\n", version);

			/* Graph Entry Creation */
			node_AP *current_node = make_node(MAC, version);

			node_AP *x = create_table_entry(current_node);
			if(x->reset_required == 0)
				n = sendto(sockfd, "SUCCES", 6, 0, (struct sockaddr *)&clientaddr, client_len);
			else {
				n = sendto(sockfd, "UPDATE", 6, 0, (struct sockaddr *)&clientaddr, client_len);
				x->reset_required = 0;
			}
		}
	}

	else {
		char temp_buf[10];
		scanf("%s", &temp_buf);
		if (strncmp("RESET", temp_buf, 5) == 0) {
			site_upgrade_counter = 1;
			site_upgrade();
			sleep(150);
		}
		else if (strncmp("KILLS", temp_buf, 5) == 0) {
			kill(ff, SIGKILL);
			exit(0);
		}
	}
	return 1;
}

void create_table(node_AP *_node_AP) {
	shm_key = SHM_KEY;
	ssize_t node_size = sizeof(node_AP);
	
	shm_id = shmget(shm_key, N_MAX_AP*node_size, IPC_CREAT | 0666);
	if (shm_id < 0) {
		perror("Cannot get shared memory");
		exit(-2);
	}
	node_table = (node_AP *)shmat(shm_id, 0, 0);
	bzero(node_table, N_MAX_AP*node_size);
}

node_AP *make_node(uint8_t *MAC, char *version) {
	node_AP *_node_AP;
	_node_AP = (node_AP *)malloc(sizeof(node_AP));
	_node_AP->reboot_lock = 0;
	_node_AP->reset_required = 0;
	bcopy(MAC, _node_AP->MAC_address, 16);
	bcopy(version, _node_AP->version, 6);
	return _node_AP;
}



node_AP *create_table_entry(node_AP *_node_AP) {
	/*check if exists*/
	int8_t _exist = -1;
	uint8_t _mac[16];
	bcopy(_node_AP->MAC_address, _mac, 16);

	for(int i = 0; i < _entries; i++) {
		uint8_t *__mac = node_table[i].MAC_address;
		if(memcmp(_mac, __mac, 16) == 0) {
			_exist = i;
			break;
		}
	}

	if(_exist == -1) {
		bcopy(_node_AP, node_table+_entries, sizeof(node_AP));
		printf("Created new entry: %d, %x%x:%x%x\n", _entries,
		       node_table[_entries].MAC_address[0], node_table[_entries].MAC_address[1],
		       node_table[_entries].MAC_address[2], node_table[_entries].MAC_address[3]);
		_entries++;
		return &node_table[_entries];
	}
	else {
		bcopy(_node_AP->version, node_table[_exist].version, 6);
		printf("Entry exists: pos %d, %x%x:%x%x, reboot reqd: %d\n", _exist,
		       node_table[_exist].MAC_address[0], node_table[_exist].MAC_address[1],
		       node_table[_exist].MAC_address[2], node_table[_exist].MAC_address[3],
		       node_table[_exist].reset_required);
		free(_node_AP);
		return &node_table[_exist];
	}
}
		
int neighbor_table() {
	int shm_id = shmget(NEIGHBOR_SHM, 100*100*sizeof(int8_t), IPC_CREAT | 0666);
	int8_t (*adj_matrix)[100] = shmat(shm_id, 0, 0);
	bzero(adj_matrix, 100*100*sizeof(int8_t));

	uint32_t _random;
	for(int i = 0; i < 100; i++) {
		for(int k = 0; k < 4; k++) {
			getrandom(&_random, sizeof(uint32_t), 0);
			int j = _random%100;
			adj_matrix[i][j] = 1;
		}
	}
	return 0;
}


//redundant 
void *lookup(uint8_t *MAC, uint8_t code) {

	int n;
	for(int i = 0; i < _entries; i++) {
		uint8_t *_mac = node_table[i].MAC_address;
		if(memcmp(_mac, MAC, 16) == 0) {
			n = i;
			break;
		}
	}

	int *rr;
	switch (code) {
	case 4:
		rr = &node_table[n].reset_required;
		return (void *)rr;
	default:
		return NULL;
	}
}


void site_upgrade() {
	int n_shm_id = shmget(NEIGHBOR_SHM, 100*100*sizeof(int8_t), 0666);
	int8_t (*n_table)[100] = shmat(n_shm_id, 0, 0);

	int shm_id = shmget(SHM_KEY, N_MAX_AP*sizeof(node_AP), 0666);
	node_AP *node_table = (node_AP *)shmat(shm_id, 0, 0);

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

	printf("Site upgrade triggered. Latest version: %s, Upgrade Cycle: %d\n", buf, site_upgrade_counter);
	
	uint8_t _remaining = 100;
	for(int i = 0; i < 100; i++) {
		node_table[i].reset_required = 1;

		/*
		if(strcmp(node_table[i].version, buf)) {
			if(node_table[i].reboot_lock == 0) {
				node_table[i].reset_required = 1;
				_remaining--;

				//+1 neighbors reboot_lock
				for(int j = 0; j < 100; j++) {
					if(n_table[i][j] == 1)
						node_table[j].reboot_lock++;
				}
			}
		}

		*/
		
	}
	site_upgrade_counter++;
	/*
	if(_remaining) {
		sleep(200);
		site_upgrade();
	}
	*/
}
