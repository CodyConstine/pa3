#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>

#define MAXBUFSIZE 1024

void * responseHandler(void*);


int main(int agrc, char* argv[])
{
    int socket_server;
    int port = atoi(argv[1]);



    int client_sock , c , *new_sock;
    struct sockaddr_in server , client;

    //Create socket
    socket_server = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_server == -1)
    {
       printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( port );

    //Bind
    if( bind(socket_server,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
       //print the error message
       printf("bind failed. Error");
       return 1;
    }

    //Listen
    listen(socket_server , 3);
    int counter = 0;
    //Accept and incoming connection
    printf("Server Finished Setting up waiting for connection\n");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_server, (struct sockaddr *)&client, (socklen_t*)&c)) != -1)
    {
        printf("\nOpening Thread %d for client request\n", counter);

        pthread_t sniffer_thread;
        int dummy = 1;
        new_sock = &dummy;
        *new_sock = client_sock;
        //spawn new threads
        if( pthread_create( &sniffer_thread , NULL ,  responseHandler , (void*) new_sock) < 0)
        {
          perror("could not create thread");
          return 1;
        }

        //counter to number threads
        counter++;
    }

    if (client_sock < 0)
    {
      printf("accept failed");
      return 1;
    }

    printf("Server is shutting down.\n");
    int test = 1;
    setsockopt(client_sock,SOL_SOCKET,SO_REUSEADDR,&test,sizeof(int));
    close(client_sock);
    free(new_sock);
    //free all dynamic varibles
    return 0;
}


void *responseHandler(void * socket_desc){//the functions for the threads to run
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[2000];


    //Receive a message from client
    read_size = recv(sock , client_message , 2000 , 0);
    printf("Recieved client message: %s\n", client_message);
    return 0;
}
