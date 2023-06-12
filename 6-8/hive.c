#include <stdio.h>     
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>   
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAXBEES 8

pthread_mutex_t mutex;
int *forest;
int found = 0;
int multicast_sock;
struct sockaddr_in multicastAddr;
char msg[32];

typedef struct thread_args {
    int socket;
    int sectors_count;
} thread_args;

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

void *beeThread(void *args) {
    int server_socket;
    int client_length;
    pthread_t threadId;
    struct sockaddr_in client_addr;
    pthread_detach(pthread_self());
    server_socket = ((thread_args*)args)->socket;
    int sectors_count = ((thread_args*)args)->sectors_count;
    free(args);
    client_length = sizeof(client_addr);
    int buffer[2];
    int selected_sector = -1;
    int hasPooh = 0;
    recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, &client_length);
    for(;;) {
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < sectors_count; i++) {
            if (forest[i] != 1) {
                selected_sector = i;
                hasPooh = forest[i] == 2;
                forest[i] = 1;
                break;
            }
        }
        snprintf(msg, sizeof(msg), "Checking sector %d\n", selected_sector);
        sendto(multicast_sock, msg, sizeof(msg), 0, (struct sockaddr*) &multicastAddr, sizeof(multicastAddr));
        pthread_mutex_unlock(&mutex);
        buffer[0] = selected_sector;
        buffer[1] = hasPooh;
        sendto(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, sizeof(client_addr));
        if (selected_sector == -1) break;
        recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &client_addr, &client_length);
        pthread_mutex_lock(&mutex);
        if (buffer[0] == 1) {
            printf("Found pooh\n");
            snprintf(msg, sizeof(msg), "Found pooh\n");
            sendto(multicast_sock, msg, sizeof(msg), 0, (struct sockaddr*) &multicastAddr, sizeof(multicastAddr));
            for (int i = 0; i < sectors_count; i++) {
                forest[i] = 1;
            }
        } else {
            snprintf(msg, sizeof(msg), "Not found pooh\n");
            sendto(multicast_sock, msg, sizeof(msg), 0, (struct sockaddr*) &multicastAddr, sizeof(multicastAddr));
        }
        pthread_mutex_unlock(&mutex);
        selected_sector = -1;
    }
}

int createTCPSocket(unsigned short server_port) {
    int server_socket;
    struct sockaddr_in server_addr;

    if ((server_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) DieWithError("socket() failed");
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;              
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
    server_addr.sin_port = htons(server_port);

    if (bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) DieWithError("bind() failed");
    printf("Open socket on %s:%d\n", inet_ntoa(server_addr.sin_addr), server_port);
    return server_socket;
}

int main(int argc, char *argv[])
{
    unsigned short server_port;
    int sectors_count;
    int pooh_index;
    int server_socket;
    pthread_t threadId;
    pthread_mutex_init(&mutex, NULL);
    char *multicastIP;
    unsigned short multicastPort;

    if (argc != 6)
    {
        fprintf(stderr, "Usage:  %s <Mulicast addr> <Multicast port> <Port for bees> <Sectors count> <Pooh location>\n", argv[0]);
        exit(1);
    }
    multicastIP = argv[1];
    multicastPort = atoi(argv[2]);
    server_port = atoi(argv[3]);
    sectors_count = atoi(argv[4]);
    pooh_index = atoi(argv[5]);

    server_socket = createTCPSocket(server_port);
    forest = (int*) malloc(sizeof(int) * sectors_count);
    for (int i = 0; i < sectors_count; i++) {
        forest[i] = 0;
    }

    forest[pooh_index] = 2;


    for (int i = 0; i < MAXBEES; i++) {
         thread_args *args = (thread_args*) malloc(sizeof(thread_args));
        args->socket = server_socket;
        args->sectors_count = sectors_count;
        if (pthread_create(&threadId, NULL, beeThread, (void*) args) != 0) DieWithError("pthread_create() failed");
    }

    if ((multicast_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

   
    int multicastTTL = 1;
    if (setsockopt(multicast_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &multicastTTL, 
          sizeof(multicastTTL)) < 0)
        DieWithError("setsockopt() failed");

    memset(&multicastAddr, 0, sizeof(multicastAddr));   
    multicastAddr.sin_family = AF_INET;                 
    multicastAddr.sin_addr.s_addr = inet_addr(multicastIP);
    multicastAddr.sin_port = htons(multicastPort); 


    printf("Open multicast socket on %s:%d\n", inet_ntoa(multicastAddr.sin_addr), multicastPort);


    for (;;) {
        sleep(1);
    }
    free(forest);
    pthread_mutex_destroy(&mutex);
    return 0;
}