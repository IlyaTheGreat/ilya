#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

void *udp_listener(void *arg)
{
	int recieved_bytes, sock, addr_size, port_available = 10;
	char buf[4];
	char *addr;
	struct sockaddr_in sockaddr_in, client_addr, local_addr;
	struct hostent *ht;

	socklen_t fromlen;
	pthread_t thread;

	char *allowsend = ALLOW_SEND;
	char *denysend = DENY_SEND;
       
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		error("socket() failed");
	}

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(16000);
	local_addr.sin_addr.s_addr = INADDR_ANY;
 
	if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
	{
		printf("cherche port\n");
		int i = 0;
		while(i < port_available)
		{
			i++;
			local_addr.sin_port = htons(16000 + i);
			if (!bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)))
                		break;
		}
	}

	addr_size = sizeof(struct sockaddr_in);
 	printf("client listen broadcast\n");
	tcp_send.allow = 0;
	while (1)
	{
		if ((recieved_bytes = recvfrom(sock, &buf[0], sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &addr_size)) < 0)//???
		{
			error("recvfrom");
		}
		hst = gethostbyaddr((char *)&client_addr.sin_addr, 4, AF_INET);
		addr = (char*)inet_ntoa(client_addr.sin_addr);
		buf[recieved_bytes] = 0;

		if(!strcmp(buf, allowsend))
		{
			if(tcp_send.allow == 0)
			{
				pthread_mutex_lock(&tcp_send.mutex);
				tcp_send.allow = 1;
				pthread_mutex_unlock(&tcp_send.mutex);
				pthread_create(&thread, NULL, tcp_sender, (void*)addr);
			}
		}
		if (!strcmp(buf, denysend))
		{ 
			pthread_mutex_lock(&tcp_send.mutex);
			tcp_send.allow = 0;
			pthread_mutex_unlock(&tcp_send.mutex);
			pthread_cancel(thread);
		}
	}
}
 
void *tcp_sender(void *arg)
int main()
{
	return 0;
}
