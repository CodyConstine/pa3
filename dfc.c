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

//thread handlers
int listHandler();
int getHandler(char*);
int putHandler(char*);

//file parser functions
char * getUser();
char * getPassword();
char * getServerPort(int);


int main(int argc, char * argv[])
{


    if(argc != 2)
    {
        printf("Correct Usage: ./dfc <dfc.conf>\n");
    }
	char command[MAXBUFSIZE];
    char * newline;
	char * pch;
	while(1)
	{
		*command = '\0';
		printf("> ");

		fgets( command, MAXBUFSIZE, stdin);
		newline = strchr(command, '\n'); // get rid of newline character
		if(newline != NULL) *newline = '\0';
	    pch = strtok (command," ");
        if(pch == NULL)
        {
            printf("No command recieved\n");
        }
		else if( strncmp(pch, "put", 3) == 0)
		{
            pch = strtok (NULL, " ");
            int responce = putHandler(pch);
            printf("Put Handler returned: %d\n", responce);
		}
		else if(strncmp(pch, "get", 3) == 0)
		{
            pch = strtok (NULL, " ");
            int responce = getHandler(pch);
            printf("Get Handler returned: %d\n", responce);
		}
		else if(strncmp(pch, "list", 2) == 0)
		{
            int responce = listHandler();
            printf("List Handler returned: %d\n", responce);
		}
		else
			printf("%s is an invalid command.\n", command);

	}
    return 0;
}


int listHandler()
{
    printf("LIST command recieved.\n");
    return 0;
}
int getHandler(char * file)
{
    printf("GET command recieved: %s\n", file);
    return 0;
}
int putHandler(char * file)
{
    int sockfd, portno;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    //char buffer[256];
    portno = 10000;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf("ERROR opening socket\n");
    server = gethostbyname("127.0.0.1");
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        printf("ERROR connecting\n");
    printf("PUT command recieved: %s\n", file);
    char * message = (char*)malloc(strlen("put ")+strlen(file)+1);
    strcpy(message, "put ");
    strcat(message, file);
    printf("Sending file message: %s\n", message);
    send(sockfd, message, strlen(message), 0);
    return 0;
}
