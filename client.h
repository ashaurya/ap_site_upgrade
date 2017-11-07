#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <time.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>

#define PORTNO 49432
#define BUFFER 8192
#define SHM_KEY 5467971

uint8_t *MAC;
char *version;

uint8_t *gen_mac();
void ap_boot();
int client_init();
