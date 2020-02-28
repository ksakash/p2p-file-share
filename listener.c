#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <linux/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>

#define IP_FOUND "IP_FOUND"
#define IP_FOUND_ACK "IP_FOUND_ACK"
#define PORT 9999
#define RCV_BUF_LEN	256

#define DEBUG_LEVEL 1
#define DEBUG(level, fmt,...) if (level <= DEBUG_LEVEL) fprintf(stderr, fmt, ##__VA_ARGS__)

struct Packet {
	int id;
	char mac[12];
	int ip;
	int port;
};

typedef struct Packet Packet;

int recvall(int handle, char * buf, int sz, struct sockaddr_in *addr)
{
	int rv;
	socklen_t addr_len = sizeof(*addr);
	DEBUG(5, "Enter recvall %d\n", handle);
	while(1)
	{
		rv = recvfrom(handle, buf, sz, 0, (struct sockaddr*)addr, &addr_len);
		if (rv < 0)
		{
			if (errno == EINTR)
			    continue;
			perror("Error encountered in recvall: ");
			DEBUG(5, "Exit recvall %d -1\n", handle);
			return -1;
		}
		break;
	}
	DEBUG(5, "Exit recvall %d %d\n", handle, rv);
	return rv;
}

int main()
{
  int sock;
  int yes = 1;
  struct sockaddr_in client_addr;
  struct sockaddr_in server_addr;
  int addr_len;
  int count;
  int ret;
  fd_set readfd;
  char buffer[1024];
  char buf[RCV_BUF_LEN];

  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0)
  {
    perror("sock error\n");
    return -1;
  }

  addr_len = sizeof(struct sockaddr_in);

  memset((void *)&server_addr, 0, addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  // server_addr.sin_addr.s_addr = inet_addr ("172.23.64.243");
  server_addr.sin_port = htons(PORT);

  ret = bind(sock, (struct sockaddr *)&server_addr, addr_len);
  if (ret < 0)
  {
    perror("bind error\n");
    return -1;
  }
  while (1)
  {
    FD_ZERO(&readfd);
    FD_SET(sock, &readfd);

    ret = select(sock + 1, &readfd, NULL, NULL, 0);
    if (ret > 0)
    {
      if (FD_ISSET(sock, &readfd))
      {
        // count = recvfrom(sock, buffer, 1024, 0, (struct sockaddr *)&client_addr, &addr_len);
        if ((count = recvall(sock, (char*)&buf, RCV_BUF_LEN-1,
						                                          (struct sockaddr_in *)&client_addr)) == -1) {
          perror("recvall");
          exit(1);
        }
        Packet *p = (Packet *)buf;
        printf ("server address: %s", inet_ntop(p->ip));
        // if (strstr(buffer, IP_FOUND))
        if (1)
        {
          // printf("\nClient message: %s", buffer);
          printf("\nClient connection information:\n\t IP: %s, Port: %d\n",
                 inet_ntop(client_addr.sin_addr), ntohs(client_addr.sin_port));

          memcpy(buffer, IP_FOUND_ACK, strlen(IP_FOUND_ACK) + 1);
          count = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&client_addr, addr_len);
        }
      }
    }
  }
}
