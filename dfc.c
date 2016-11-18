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

	char *newline = NULL;  //to remove new
	char command[MAXBUFSIZE];
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
    printf("PUT command recieved: %s\n", file);
    return 0;
}
