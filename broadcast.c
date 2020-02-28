#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <linux/in.h>
#include <sys/socket.h>
#include <sys/select.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999
#define BEACON_INTERVAL 5

struct Packet {
	int id;
	char mac[12];
	int ip;
	int port;
};

typedef struct Packet Packet;

int main()
{
  printf ("client init\n");
  int sock;
  int yes = 1;
  struct sockaddr_in broadcast_addr;
  struct sockaddr_in server_addr;
  int addr_len;
  int count;
  int ret;
  fd_set readfd;
  char buffer[1024];
  int i;

  Packet p;

	p.id = 1;
	strcpy(p.mac, "ABCDEFGHIJKL");
	p.ip = inet_addr("192.168.100.102");
	p.port = htonl((long)9999);

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
  {
    perror("sock error");
    return -1;
  }
  ret = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&yes, sizeof(yes));
  if (ret == -1)
  {
    perror("setsockopt error");
    return 0;
  }

  addr_len = sizeof(struct sockaddr_in);

  memset((void *)&broadcast_addr, 0, addr_len);
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  broadcast_addr.sin_addr.s_addr = inet_addr ("192.168.100.255"); // address of the other endpoint
  broadcast_addr.sin_port = htons(PORT);

  while (1)
  {
    // ret = sendto(sock, IP_FOUND, strlen(IP_FOUND), 0, (struct sockaddr *)&broadcast_addr, addr_len);
    if ((ret=sendto(sock, &p, sizeof(Packet), 0,
						(struct sockaddr *)&broadcast_addr, addr_len)) == -1) {
			perror("sendto");
			exit(1);
		}
    printf ("packets sent\n");

    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);

    ret = select(sock + 1, &readfd, NULL, NULL, NULL);

    if (ret > 0)
    {
      if (FD_ISSET(sock, &readfd))
      {
        count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&server_addr, &addr_len);
        printf("\trecvmsg is %s\n", buffer);
        if (strstr(buffer, IP_FOUND_ACK))
        {
          printf("\tfound server IP is %s, Port is %d\n", inet_ntop(server_addr.sin_addr),
                                                                    htons(server_addr.sin_port));
        }
      }
    }
    sleep(BEACON_INTERVAL);
  }
}
