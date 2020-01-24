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
#define RCV_BUF_LEN 256

int main () {

    int sockfd;
    struct sockaddr_in my_addr;
    struct sockaddr_in their_addr;
    socklen_t addr_len;

    int numbytes;
    char buf[RCV_BUF_LEN];
    char print_buf[256];
    printf ("Started listener!!\n");

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf ("Unable to create a socket");
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(BROADCAST_PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1) {
        printf ("Error in binding the socket with file descriptor");
        exit(1);
    }

    addr_len = sizeof their_addr;

    while (1) {
        if ((numbytes = recvfrom(sockfd, &buf, RCV_BUF_LEN-1, 0, (struct sockaddr *)&their_addr,
                                                                            &addr_len)) == -1) {
            printf ("recvall");
            exit(1);
        }

        printf ("got packet from %s\n", inet_ntoa(their_addr.sin_addr));
        printf ("packet is %d bytes long\n", numbytes);
    }

    close (sockfd);

    return 0;
}
