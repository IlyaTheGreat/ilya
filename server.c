#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

struct datebase
{
    char ini[4];
    char dt[25];
};

int main()
{
    int sock, listener;
    struct sockaddr_in addr;
    char buf[4];
    char * data = malloc(25*sizeof(char));
    int bytes_read;
    struct datebase dbase[10];

    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("socket");
        exit(1);
    }
    
    FILE * fn = fopen("db.txt", "r");
    for (int i=0; i<10; i++)
    {
        fgets(dbase[i].ini, 4, fn);
        fgets(dbase[i].dt, 25, fn);
        printf("%s, %s", dbase[i].ini, dbase[i].dt);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
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
            else printf("\nrecieved key: %s\n", buf );
            int flag = 0;
            for(int i =0; i<10; i++)
            {
            	if(strcmp(buf,dbase[i].ini) == 0)
            		{
            			data = dbase[i].dt;
            			printf("sending%s",data);
            			flag =1;
            			break;
            			
            		}
            }
             if (flag == 0)
             	data = "error:not existed data.\n";
            send(sock, data, strlen(data), 0);
        }
        close(sock);
    }   
    return 0;
}