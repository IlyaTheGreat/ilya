#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

struct messager
{
    char uname[15];
    char message[256];
};

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

    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listener, 1);
    
    while(1)
    {
        sock = accept(listener, NULL, NULL);
        if(sock < 0)
        {
            perror("accept");
            exit(3);
        }
        while(1)
        {
            bytes_read = recv(sock, buf, 1024, 0);
            if(bytes_read <= 0) break;
            else printf("\n%s: %s\n", uname, message);
            send(sock, message, strlen(message), 0);
        }
        close(sock);
    }   
    return 0;
}