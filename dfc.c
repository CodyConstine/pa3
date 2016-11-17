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
void * listHandler(void *);
void * getHandler(void *);
void * putHandler(void *);

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
    pthread_t resovlerThread[4];
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
            printf("PUT command recieved: %s\n", pch);
            int i = 0;
            for(i = 0; i<4; i++)
            {
                if( pthread_create( &resovlerThread[i] , NULL ,  putHandler , (void*) &i) < 0)
                {
                  perror("could not create thread");
                  return 1;
                }
            }
		}
		else if(strncmp(pch, "get", 3) == 0)
		{
            int i =0;
            for(i = 0; i<4; i++)
            {
                if( pthread_create( &resovlerThread[i] , NULL ,  getHandler , (void*) &i) < 0)
                {
                  perror("could not create thread");
                  return 1;
                }
            }
            pch = strtok (NULL, " ");
            printf("GET command recieved: %s\n", pch);
		}
		else if(strncmp(pch, "list", 2) == 0)
		{
            int i = 0;
            for(i = 0; i<4; i++)
            {
                if( pthread_create( &resovlerThread[i] , NULL ,  listHandler , (void*) &i) < 0)
                {
                  perror("could not create thread");
                  return 1;
                }
            }
            pch = strtok (NULL, " ");
            printf("LIST command recieved: %s\n", pch);
		}
		else
			printf("%s is an invalid command.\n", command);

	}
    return 0;
}
