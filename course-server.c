#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#define maxlen 100

struct messager
{
    char uname[15];
    char message[maxlen];
};

struct queue
{
	pthread_mutex_t mutex;
	int elements;
	char packet[5][maxlen];
} qpacket = {PTHREAD_MUTEX_INITIALIZER};

void *broadcaster(void*);//вещатель по состоянию очереди

void *listentcp(void *arg)//приемник входящих подключений клиентов
{
 	int sockfd, newsockfd;
 	socklen_t clilen;
 	struct sockaddr_in serv_addr, cli_addr;
 		
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	if (sockfd < 0) 
 	{
 	       error("socket() failed");
 	}
 		 
 	bzero((char *) &serv_addr, sizeof(serv_addr));
 
 	serv_addr.sin_family = AF_INET;
 	serv_addr.sin_addr.s_addr = INADDR_ANY;
 	serv_addr.sin_port = htons(33000);
 
 	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
 	{
 		error("bind() failed");
 	}
 
 	listen(sockfd,5);
 	clilen = sizeof(cli_addr);
 
 	while (1) 
 	{
 		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
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

void *tcpclient(void*);//обработчик входящих подключений клиентов

void *listentcp_manager(void *arg)//приемник входящих подключений менеджеров
{
	int sockfd, newsockfd;
 	socklen_t clilen;
 	struct sockaddr_in serv_addr, cli_addr;
 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
 	if (sockfd < 0) 
 	{
 	       error("socket() failed");
 	}
 	 
 	bzero((char *) &serv_addr, sizeof(serv_addr));
 	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(33000);
 
 	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
 	{
 		error("bind() failed");
 	}
 
 	listen(sockfd, 5);
 	clilen = sizeof(cli_addr);
 	while (1) 
 	{
 		newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
 		if (newsockfd < 0) 
 		{
 			error("accept() failed");
 		}
 		printf("new manager\n");
 		pthread_t thread;
 	       	pthread_create(&thread, NULL, tcpmanager, (void*)&newsockfd);
 	}
 
 	    close(newsockfd);
 	    close(sockfd);
}
void *tcpmanager(void*);//обработчик входящих подключений менеджеров

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
int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    char buf[256];
    char * message = malloc(256*sizeof(char));
    int bytes_read;
    struct messager chat[50];

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }

//Открываем 20 портов (10 для менеджеров, 10 для клиентов)
    addr.sin_family = AF_INET;
    for(int i=0; i<20; i++)
        addr.sin_port = htons(3425 + i);
    return 0;
}
