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
    int sock;
    struct sockaddr_in multicastAddr;
    unsigned short multicastPort;
    char* multicastIP;

    if (argc != 3)
    {
       fprintf(stderr, "Usage: %s <Multicast IP> <Multicast Port>\n", argv[0]);
       exit(1);
    }

    multicastIP = argv[1];
    multicastPort = atoi(argv[2]);
    
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) DieWithError("socket() failed");

    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    memset(&multicastAddr, 0, sizeof(multicastAddr));   
    multicastAddr.sin_family = AF_INET;                 
    multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    multicastAddr.sin_port = htons(multicastPort);

    if (bind(sock, (struct sockaddr *) &multicastAddr, sizeof(multicastAddr)) < 0) DieWithError("bind() failed");

    struct ip_mreq multicastRequest;
    inet_aton(multicastIP, &(multicastRequest.imr_multiaddr));
    multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicastRequest, sizeof(multicastRequest)) < 0) DieWithError("setsockopt() failed");

    char msg[32];
    for (;;) {
        recvfrom(sock, msg, sizeof(msg), 0, NULL, 0);
        printf("%s", msg);
    }
    close(sock);
    return 0;
    return 0;
}