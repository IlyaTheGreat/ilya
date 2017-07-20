#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <strings.h>
#include <signal.h>

void *tcp_reciever(void *arg)
{
 	int my_sock;
 	int sleep_time, len;
 	char buff[100];
 	struct sockaddr_in serv_addr;
 	int recieved_bytes;
 	struct hostent *server;
 	char *addr;

 	addr = (char*)arg;
 
 	printf("CLIENT 2 type: start.. connecting to %s:%d\n", addr, SERVPORT);
 
 	my_sock = socket(AF_INET, SOCK_STREAM, 0);
 	if (my_sock < 0)
 		error("socket() failed");
 	server = gethostbyname(addr);
 	if (server == NULL) 
 	{
 		fprintf(stderr,"gethostbyname() failed\n");
 		exit(0);
 	}
 
 	bzero((char *) &serv_addr, sizeof(serv_addr));
 
 	serv_addr.sin_family = AF_INET;
 
 	bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
 	serv_addr.sin_port = htons(SERVPORT);
 
 	if (connect(my_sock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
	{
		tcp_recv.allow = 1;
		perror("connect() failed");
		pthread_exit(0);
	}
	else
		printf("manager connected\n\n");
 	while (tcp_recv.allow == 1)
	{
 		fflush(stdout);
 		bzero(&buff, MAXMSGLEN);
		bytes_recv = recv(my_sock,(char*)&buff[0], MAXMSGLEN, 0);
		if (bytes_recv <= 0) 
		error("recv() failed");
		AMessage *pmsg;
		pmsg = amessage__unpack (NULL, bytes_recv, buff);
		if (pmsg == NULL)
		{
			fprintf(stderr, "error unpacking incoming message\n");
			return (void*)1;
		}
		len = pmsg->b;
		sleep_time = pmsg->a;
		printf("CLIENT 2 type: recieved %d bytes: <=== ",bytes_recv);
			
		printf("[%d][%d][%s]\n", pmsg->a, pmsg->b, pmsg->c);
		amessage__free_unpacked(pmsg,NULL);

		printf("CLIENT 2 type: sleep %d sec\n\n", sleep_time);
 		sleep(sleep_time);
 		}
 		printf("CLIENT 2 type: queue is empty\n");
 		close(my_sock);
 		pthread_exit(0);
 	}


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
		tcp_recv.allow = 1;
		perror("socket() failed");
		exit(0);
	}

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(17000);
	local_addr.sin_addr.s_addr = INADDR_ANY;
 
	if (bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
	{
		printf("cherche port\n");
		int i = 0;
		while(i < port_available)
		{
			i++;
			local_addr.sin_port = htons(17000 + i);
			if (!bind(sock, (struct sockaddr *)&local_addr, sizeof(local_addr)))
                		break;
		}
	}

	addr_size = sizeof(struct sockaddr_in);
 	printf("manager listen broadcast\n");
	tcp_send.allow = 0;
	while (1)
	{
		if ((recieved_bytes = recvfrom(sock, &buf[0], sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &addr_size)) < 0)//???
		{
			tcp_recv.allow = 1;
			perror("recvfrom");
			exit(0);
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

int main(int argc, char *argv[])
{
	printf("manager has started\n");
	pthread_t udp_listener_thread, tcp_reciever_thread;
	pthread_create(&udp_listener_thread, NULL, udp_listener, NULL);
	pthread_join(udp_listener_thread, NULL);
	return 0;
}
