#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <pthread.h>

#define maxlen 100
#define ALLOW_RECIEV "RCV"
#define DENY_RECIEV "RDN"
#define ALLOW_SEND "ALL"
#define DENY_SEND "DEN"

struct queue
{
	pthread_mutex_t mutex;
	int elements;
	char packet[5][maxlen];
} qpacket = {PTHREAD_MUTEX_INITIALIZER};


void *broadcaster(void *arg)//вещатель по состоянию очереди
{
	int sock, n, port, K;
	char *msgf, *msgs, *ip;
	unsigned int length;
	struct sockaddr_in server;
	struct hostent *hp;
 	ip = ((char**)arg)[1];
	K = atoi(((char**)arg)[2]);
	fflush(stdout);
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
	{
		perror("socket() failed");
		exit(1);
	}
	server.sin_family = AF_INET;
	hp = gethostbyname(ip);
 	if (hp == 0) 
	{
		error("gethostbyname() failed");
	}
	bcopy((char *)hp->h_addr_list[0], (char *)&server.sin_addr, hp->h_length);
	length = sizeof(struct sockaddr_in);
 
	while(1)
	{	
		port = 16000;
		if(qpacket.elements < 5)
		{
			msgf = ALLOW_SEND;
		}
		else
		{
			msgf = DENY_SEND;
		}
		server.sin_port = htons(port);
		for(int i = 0; i < 10; i++)
		{
			if ((n = sendto(sock,msgf,ALDENLEN,0,(const struct sockaddr *)&server,length)) < 0) 
			{
				error("sendto() failed");
			}
			server.sin_port = htons(port + i);
		}

		port = 17000;	
		if(qpacket.elements > 0)
		{
			msgs = ALLOW_RECIEV;
		}
		else
		{
			msgs = DENY_RECIEV;
		}
		server.sin_port = htons(port);
		for(int i = 0; i < 10; i++)
		{
 			if ((n = sendto(sock,msgs,ALDENLEN,0,(const struct sockaddr *)&server,length)) < 0) 
			{
				error("sendto() failed");
			}
		server.sin_port = htons(port+i);
		}	
		sleep(K);
	}
}
void *tcpclient(void *arg)//обработчик входящих подключений клиентов
{
	
}

void *tcpmanager(void *arg)//обработчик входящих подключений менеджеров
{

}

void *listentcp_manager(void *arg)//приемник входящих подключений менеджеров
{
	int n, len, sleep_time;
 	char *message;
 
 	my_sock = *((int*)arg);
 	while(qpacket.elements > 0)
 	{
		msg = msg_get();
		len = ((int*)message)[1];
		for (int i = 0; i < maxlen+1; i++)
		{
			if(msg[i]=='\0'){
			len = i;
			break;
		}
	}
	AMessage *pmsg;
	pmsg = amessage__unpack (NULL, len, message);
	if (pmsg == NULL)
	{
		fprintf(stderr, "error unpacking incoming message\n");
		return (void*)1;
	}
	sleep_time = pmsg->a;
 	if ((n = send(my_sock, message, len, 0))< 0)
 	{
 		error("send() failed");
	}
 
	printf("sent %d bytes: ", len);
	printf("sent %d bytes: ===> [%d][%d][%s]\n", len, pmsg->a, pmsg->b, pmsg->c);
 
	for(int i = 8;i < len;i++) //len-1?
	{
		printf("%c", msg[i]);
	}
	printf("сells in the queue: %d\n\n", 5 - qpacket.elements);
	amessage__free_unpacked(pmsg,NULL);
	sleep(sleep_time);

	printf("\nсells in the queue: %d\n", 5 - qpacket.elements);
	sleep(((int*)message)[0]);
	}
 
 	free(message);
 	printf("no cell to sending\n");
	sleep(5);
	close(my_sock);
	pthread_exit(0);
}
void *listentcp(void *arg)//приемник входящих подключений клиентов
{
 	int sockfd, newsockfd;
 	socklen_t clilen;
 	struct sockaddr_in addr_client, addr_server;
 		
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	if (sockfd < 0) 
 	{
 	       error("socket() failed");
 	}
 		 
 	bzero((char *) &addr_server, sizeof(addr_sever));
 
 	addr_server.sin_family = AF_INET;
 	addr_server.sin_addr.s_addr = INADDR_ANY;
 	addr_server.sin_port = htons(33000);
 
 	if (bind(sockfd, (struct sockaddr *) &addr_server, sizeof(addr_server)) < 0)
 	{
 		error("bind() failed");
 	}
 
 	listen(sockfd,5);
 	clilen = sizeof(addr_client);
 
 	while (1) 
 	{
 		newsockfd = accept(sockfd,(struct sockaddr *) &addr_client, &clilen);
 		if (newsockfd < 0) 
 		{
 			error("accept() failed");
 		}
 		printf("new client\n");
 		pthread_t thread;
 	       	pthread_create(&thread, NULL, tcpclient, (void*)&newsockfd);
 	    }
 	    close(newsockfd);
 	    close(sockfd);
}

void msg_to_q(char *message)//помещение сообщения в очередь
{
	int flag = 0;
	if (qpacket.elements == 5)
	{
		printf("очередь переполнена\n");
		flag = -1;
	}
	pthread_mutex_lock(&qpacket.mutex);
	bcopy(pt, qpacket.packet[qpacket.elements], maxlen);
	bcopy(pt, qpacket.packet[qpacket.elements], maxlen+12);
	qpacket.elements++;
	pthread_mutex_unlock(&qpacket.mutex);
	return flag;
}
char *msg_get() 
{
	char *pt = (char*) malloc(sizeof(char) * maxlen);
	pthread_mutex_lock(&qpacket.mutex);
	if(qpacket.elements == 0)
	{
		return pt;
	}
	bcopy(qpacket.packet[0], pt, maxlen);
	qpacket.elements; 
	bzero(qpacket.packet[0], maxlen);
	for(int i = 0; i < qpacket.elements-1; i++)
	{
		bcopy(qpacket.packet[i+1], qpacket.packet[i], maxlen);
	}

 	qpacket.elements--;
	pthread_mutex_unlock(&qpacket.mutex);
	return pt;
}

int main(int argc, char *argv[])
{
	if(argc != 3) 
	{
		exit(1);
	}
	qpacket.elements = 0;

	pthread_t udp_broadcast_thread, tcp_recieve_listener_thread, tcp_send_listener_thread;
	pthread_create(&udp_broadcast_thread, NULL, udp_broadcast, (void*)argv);
	pthread_create(&tcp_recieve_listener_thread, NULL, tcp_recieve_listener, NULL);
	pthread_create(&tcp_send_listener_thread, NULL, tcp_send_listener, NULL);
	pthread_join(udp_broadcast_thread, NULL);
	 
	return 0;
}
//Открываем 20 портов (10 для менеджеров, 10 для клиентов)
    addr.sin_family = AF_INET;
    for(int i=0; i<20; i++)
        addr.sin_port = htons(3425 + i);
    return 0;
}
