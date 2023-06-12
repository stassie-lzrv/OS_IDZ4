#include <stdio.h>   
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void DieWithError(char *errorMessage)
{
    perror(errorMessage);
    exit(1);
}

int main(int argc, char *argv[])
{
    int client_socket;
    struct sockaddr_in server_addr;
    unsigned short server_port;
    int recv_msg_size;
    char *server_ip;

    if (argc != 3)
    {
       fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
       exit(1);
    }

    server_ip = argv[1];
    server_port = atoi(argv[2]);

    if ((client_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) DieWithError("socket() failed");

    memset(&server_addr, 0, sizeof(server_addr));  
    server_addr.sin_family      = AF_INET;        
    server_addr.sin_addr.s_addr = inet_addr(server_ip); 
    server_addr.sin_port        = htons(server_port);

    int server_len = sizeof(server_addr);
    int buffer[2];
    sendto(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
    
    for(;;) {
        recvfrom(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_addr, &server_len);
        int sector = buffer[0];
        int hasPooh = buffer[1];
        if (sector == -1) {
            printf("No free sectors to check\n");
            break;
        }
        printf("Checking sector #%d\n", sector);
        sleep(2 + rand() % 5);
        if (hasPooh) {
            printf("Found the Pooh in sector #%d\n", sector);
            buffer[0] = 1;
        } else {
            printf("Did not found the Pooh in sector #%d\n", sector);
            buffer[0] = -1;
        }
        sendto(client_socket, buffer, sizeof(buffer), 0, (struct sockaddr*) &server_addr, sizeof(server_addr));
    }
    close(client_socket);
    return 0;
}