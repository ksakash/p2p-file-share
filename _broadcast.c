#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BROADCAST_PORT 4950
#define BEACON_INTERVAL 10

struct Packet {
    int id;
    char mac[12];
    int ip;
    int port;
};

typedef struct Packet Packet;

int main () {

    Packet pkt;

    pkt.id = 1;
    strcpy (pkt.mac, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    pkt.ip = inet_addr ("128.208.4.14");
    pkt.port = htonl ((long)2190);

    int sockfd; // socket file descriptor
    struct sockaddr_in their_addr; // connector's address information
    int numbytes;
    int broadcast = 1;

    unsigned int broadcast_addr = pkt.ip | 0xFF000000;

    // create the socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf ("Not able to create socket");
        exit(1);
    }

    // this allows sending packets to the broadcast address
    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof broadcast) == -1) {
        printf ("Enabling socket to broadcast failed!!");
        exit(1);
    }

    // fill info about the server (address to send the packet to)
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(BROADCAST_PORT);
    their_addr.sin_addr.s_addr = broadcast_addr;
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    // send the broadcast packet
    while (1) {
        if ((numbytes=sento(sockfd, pkt, sizeof(Packet), 0, (struct sockaddr *)&their_addr,
                                                                sizeof their_addr)) == -1) {
            printf ("Failed to send packet");
            exit (1);
        }
        printf("broadcast(): sent %d bytes to %s\n", numbytes, inet_ntoa(their_addr.sin_addr));
        sleep(BEACON_INTERVAL);
		printf("broadcast(): thread awake\n");
    }
    close(sockfd);

    return 0;

}
