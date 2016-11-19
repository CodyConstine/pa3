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

char * root;

void * responseHandler(void*);
int listHandler(int);
int getHandler(char*, int, char*);
int putHandler(char*, int);

int checkdirectory(char * directory);

int sendfile(char*, int);

int checkUser(char*, char*);

int main(int agrc, char* argv[])
{
    int socket_server;
    int port = atoi(argv[2]);

    root = argv[1];
    //(char*)malloc(strlen("/home/cody/Dropbox/network/pa3")+strlen(argv[1])+1);
    //strcpy(root, "/home/cody/Dropbox/network/pa3");
    //strcat(root,argv[1]);
    printf("Root: %s\n",root);
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

	char * pch;
    char message[2000];
    char *client_message;
    char* userName;
    char *password;
    //Receive a message from client
    read_size = recv(sock , message , 2000 , 0);

    pch = strtok(message,",");
    client_message = (char*)malloc(strlen(pch)+1);
    strcpy(client_message, pch);
    pch = strtok(NULL,",");
    userName = (char*)malloc(strlen(pch)+1);
    strcpy(userName, pch);
    pch = strtok(NULL,",");
    password = (char*)malloc(strlen(pch)+1);
    strcpy(password, pch);

    printf("userName:%s password: %s\n", userName, password);
    int r = checkUser(userName, password);
    if( r == 0)
    {
        printf("userName and/or password invalid\n");
    }
    else if (r == 1)
    {
        printf("Valid userName and password\n");
        int i = checkdirectory(userName);
    }
    else
    {
        printf("An error occured\n" );
    }
    char command[MAXBUFSIZE];
    char * newline;
    int responce = -1;
	newline = strchr(command, '\n'); // get rid of newline character
	if(newline != NULL) *newline = '\0';
    pch = strtok (client_message," ");
    if(pch == NULL)
    {
        printf("No command recieved\n");
    }
	else if( strncmp(pch, "put", 3) == 0)
	{
        pch = strtok (NULL, " ");
        //int responce = putHandler(pch, sock);
        printf("Put Handler returned: %d\n", responce);
	}
	else if(strncmp(pch, "get", 3) == 0)
	{
        pch = strtok (NULL, " ");
        responce = getHandler(pch, sock, userName);
        printf("Get Handler returned: %d\n", responce);
	}
	else if(strncmp(pch, "list", 2) == 0)
	{
        //int responce = listHandler(sock);
        printf("List Handler returned: %d\n", responce);
	}
	else
		printf("%s is an invalid command.\n", command);


    printf("Recieved client message: %s\n", client_message);
    close(sock);
    free(userName);
    free(password);
    free(client_message);
    return 0;
}

int getHandler(char * file, int sock, char * user)
{
    printf("Get recieved filename: %s\n", file);

    /*
      with file given, add "." to front and look for ".1-4"im l
    */

    //memcpy(filename, cmd +4, strlen(cmd) +1);

    if(strlen(file)!=0)
    {
      /*

        Given text.txt, search for .text.txt.1-4 and find ones on server
        then send those back.

      */
      char * actualfile;
      char * f1;
      char * f2;
      char * f3;
      char * f4;
      actualfile= (char*)malloc(strlen(".")+strlen(file)+1);
      strcpy(actualfile,".");
      strcat(actualfile,file);



       char * fullfile = (char*)malloc(strlen(root)+strlen("//")+strlen(user)+strlen(actualfile)+1);
       strcpy(fullfile,root);
       strcat(fullfile,"/");
       strcat(fullfile,user);
       strcat(fullfile, "/");
       strcat(fullfile,actualfile);
       f1 = (char*)malloc(strlen(fullfile)+strlen(".1")+1);
       strcpy(f1,fullfile);
       strcat(f1,".1");
       f2 = (char*)malloc(strlen(fullfile)+strlen(".2")+1);
       strcpy(f2,fullfile);
       strcat(f2,".2");
       f3 = (char*)malloc(strlen(fullfile)+strlen(".3")+1);
       strcpy(f3,fullfile);
       strcat(f3,".3");
       f4 = (char*)malloc(strlen(fullfile)+strlen(".4")+1);
       strcpy(f4,fullfile);
       strcat(f4,".4");
       int suc=-1;
       printf("Looking For Files %s, %s, %s, %s\n",f1, f2, f3, f4);
       suc=sendfile(f1,sock);
       if(!suc)
       {
         printf("FILE %s Failed\n", f1);
       }
       suc=sendfile(f2,sock);
       if(!suc)
       {
         printf("FILE %s Failed\n", f2);
       }
       suc=sendfile(f3,sock);
       if(!suc)
       {
         printf("FILE %s Failed\n", f3);
       }
       suc=sendfile(f4,sock);
       if(!suc)
       {
         printf("FILE %s Failed\n", f4);
       }
       free(actualfile);
       free(fullfile);
       free(f1);
       free(f2);
       free(f3);
       free(f4);
    }


    return 0;
}

int checkUser(char * user, char * pass)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * pch;

    int returnStatus = -1;
    file = fopen("dfs.conf", "r");
    if (file == NULL)
    {
        printf("dfs.conf is missing\n");
        exit(EXIT_FAILURE);
    }
    int i =0;
    while ((read = getline(&line, &len, file)) != -1) {
        pch = strtok (line," ");//get indetinfer
        if(strncmp(pch, "#", 1) == 0)//any line with # will be filtered out
        {
            continue;
        }
        if(strcmp(user, pch)==0) //all others will work the same
        {
            //printf("Found user:%s\n", pch);
            pch = strtok (NULL, " "); //grab the value
            pch[strcspn(pch, "\n")] = 0; //remove /n
            //printf("Found pass:%s Known pass:%s\n", pch, pass);
            if(strcmp(pass, pch)==0)
            {
                returnStatus = 1;
            }
            else
            {
                returnStatus = 0;
            }

        }
        i++;
    }
    fclose(file);
    if (line){
       free(line);
    }
    return returnStatus;
}

int checkdirectory(char * directory)
{
    DIR* dir;
    char * fullroot = (char*)malloc(strlen(root)+strlen("/")+strlen(directory)+1);

    //strcpy(fullroot,".");
    strcpy(fullroot, root);
    strcat(fullroot,"/");
    strcat(fullroot,directory);
    printf("Full path%s\n", fullroot);
    dir = opendir(fullroot);
    //dir = opendir(directory);
    //  puts("CheckinDirectory");
    if (dir)
    {
        printf("exists");
        /* Directory exists. */
        closedir(dir);
    }
    else if (ENOENT == errno)
    {

        puts("Doesnt Exist");
        /* Directory does not exist. */

        DIR * checkroot = opendir(root);
        if(checkroot)
        {
            //root exists
        }
        else
        {
            char mkdircommand[50];
            strcpy(mkdircommand,"mkdir .");
            strcat(mkdircommand, root);
            printf("Making: %s\n", root);
            system(mkdircommand);

            //int result = mkdir(root, 0777);
        }
        puts("Making User Directory");
        char mkdircommand[50];
        strcpy(mkdircommand,"mkdir ");
        strcat(mkdircommand, fullroot);
        printf("Making: %s\n", fullroot);
        system(mkdircommand);

    //      int result = mkdir(fullroot, 0777);

    }
    else
    {
        puts("screwed up");
    /* opendir() failed for some other reason. */
    }
    return 0;
}

int sendfile(char *filename, int sock)
{
  char buffer[MAXBUFSIZE];
  int nbytes=0;
  int size=0;
  FILE * f;
  //printf("GETTING SIZE OF FILE %s\n", filename);
  f = fopen(filename, "r");

  if(f==NULL)
  {
    //sprintf(buffer, "File not found");
    //strcpy(message, buffer);
    //nbytes=write(sock,buffer, MAXBUFSIZE);
    //printf("FILE NOT FOUND %s\n", filename);
    return -1;
  }
  else
  {
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    //printf("SIZE %d\n", size);
    nbytes=write(sock,filename,MAXBUFSIZE);
    nbytes=write(sock, &size, sizeof(int));
    //printf("SENDING %s\n", filename);
    *buffer = '\0';
    while (fgets(buffer, MAXBUFSIZE, f)!=NULL)
    {
    //  printf("%s", buffer);
      nbytes=write(sock,buffer, MAXBUFSIZE);
    }

    sprintf(buffer, "Done fetching file");
  //  strcpy(message, buffer);

     nbytes=write(sock,buffer, MAXBUFSIZE);
  }
  fclose(f);
  return 1;
}
