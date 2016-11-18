#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h> //for threading , link with lpthread
#include <dirent.h>

#define BUFSIZE 8096
#define MAXBUFSIZE 100
int size(char *ptr)
{
    //variable used to access the subsequent array elements.
    int offset = 0;
    //variable that counts the number of elements in your array
    int count = 0;

    //While loop that tests whether the end of the array has been reached
    while (*(ptr + offset) != '\0')
    {
        //increment the count variable
        ++count;
        //advance to the next element of the array
        ++offset;
    }
    //return the size of the array
    return count;
}


int user_check(char * inusername, char * inpassword)
{
  FILE * file;
  char *space;
  int index;
  //bool validuser = false;
  file = fopen( "dfs.conf" , "r");
  char user[50];
  char pw[50];
  char str[1024];
  int i = 0;
  int founduser = 0;
  if (file) {
      while (fscanf(file, "%s", str)!=EOF)
      {
          //printf("instring is %s\n",str);
          space=strchr(str,' ');
          index=(int)(space-str);
          if(i==0)
          {
            sprintf(user, "%.*s", index, str);
            //printf("Username: %s\n", user);
          }
          if(i==1)
          {
            sprintf(pw, "%.*s", index, str);
            // printf("Password: %s\n", pw);
            if(!strcmp(inusername,user) && !strcmp(inpassword,pw))
            {
            //   printf("Username: %s  UsernameIN:  %s\n", user, inusername);
            //   printf("Password: %s  PasswordIN: %s\n", pw, inpassword);
               printf("Valid User\n");
               founduser = 1;
            }

          }
         //  printf("i: %d\n", i);
          //printf("founduser: %d\n", founduser);
          i ^= 1;
      }

      fclose(file);
      return founduser;
  }

}
void put(char msg[], char buf[], int sock)
{


  int nbytes;
  char filename[MAXBUFSIZE];
  FILE *f;
  int size;
  nbytes = read(sock, &size, sizeof(int));
  printf("SIZE %d\n", size);
  memcpy(filename, buf+4, strlen(buf)+1);
  if(strlen(filename)!=0)
    {
      strcat(filename, "_put.s");
      //printf("FILENAME %s\n", filename);
      bzero(buf,MAXBUFSIZE);
      //printf("zeroed")
      //printf("%s",buf);
      f = fopen(filename, "w");
      //fwrite(buf, 1, MAXBUFSIZE, f);

      char full[size];
      bzero(full, size);
      if(nbytes<0)
      {
        printf("ERROR GETTING FILE\n");
      }
      if (strcmp(buf, "File not found" ) != 0)
      {

        //fwrite(buf, 1, nbytes, f);
        while(1)
        {


            //int putsreturn=fputs(buf, f);
            //fputs(buf, f);
            //printf("PUTSRETURN %d",putsreturn);
            //fprintf(f, buf);
            bzero(buf,MAXBUFSIZE);

            nbytes = read(sock, buf, MAXBUFSIZE);
            if(strcmp(buf, "Done fetching file")==0)
            {
              printf("done\n");
              break;
            }
            strcat(full, buf);
            //f=freopen(filename, "w+", stdout);
            //printf("%s",buf);



        }
        //printf("%s PRINTED FULL",full);
        fwrite(full, 1, size, f);
        fclose(f);
      }

    }
}
void get(char message[], char cmd[], int sock)
{
  /*
    with file given, add "." to front and look for ".1-4"
  */
  int nbytes;
  char filename[MAXBUFSIZE];
  char buffer[MAXBUFSIZE];
  FILE *f;

  memcpy(filename, cmd +4, strlen(cmd) +1);

  if(strlen(filename)!=0)
  {
     f = fopen(filename, "r");
     int size;
     fseek(f, 0, SEEK_END);
      size = ftell(f);
      fseek(f, 0, SEEK_SET);

     nbytes=write(sock, &size, sizeof(int));
     if(f==NULL)
     {
       sprintf(buffer, "File not found");
       strcpy(message, buffer);
       nbytes=write(sock,buffer, MAXBUFSIZE);
     }
     else
     {
       *buffer = '\0';
       while (fgets(buffer, MAXBUFSIZE, f)!=NULL)
       {
         printf("%s", buffer);
         nbytes=write(sock,buffer, MAXBUFSIZE);
       }

       sprintf(buffer, "Done fetching file");
       strcpy(message, buffer);

        nbytes=write(sock,buffer, MAXBUFSIZE);
     }
  }

}

void list(char * root, char * user, int sock)
{
    DIR *d;
    int nbytes;
    struct dirent *dir;
    d = opendir(".");  //user

      if (d)
      {
        while ((dir = readdir(d)) != NULL)
        {
          printf("|%s|\n", dir->d_name);

          nbytes = write(sock, dir->d_name, MAXBUFSIZE);
          printf("bytes passed %d\n", nbytes);
        }
        char donesig[]="Done With LS";
        nbytes = write(sock, donesig, nbytes);
        closedir(d);
      }

}

void * connection_handle(void *  socket_d)
{
  /* Open the file that we wish to transfer */
    char user[50];
    char pw[50];
    char uspw[100];
    int socketfd;
    char message[MAXBUFSIZE];
    socketfd = *(int*)socket_d;
    long ret;
    static char buffer[BUFSIZE+1];
    pid_t processID = getpid();
    while(1)
    {
       ret =read(socketfd,buffer,BUFSIZE);
       printf("Initial Buffer |%s|\n", buffer);
       if(!strncmp(buffer,"GET ",4) || !strncmp(buffer,"get ",4) || !strncmp(buffer,"PUT ",4) || !strncmp(buffer,"put ",4) || !strncmp(buffer,"LIST",4) || !strncmp(buffer,"list",4))
       {
            if(ret > 0 && ret < BUFSIZE)  /* return code is valid chars */
              buffer[ret]=0;    /* terminate the buffer */
            else buffer[0]=0;
            for(int i=0;i<ret;i++)  /* remove CF and LF characters */
              if(buffer[i] == '\r' || buffer[i] == '\n')
                buffer[i]='\0';
            printf("Buffer From Client |%s|\n", buffer);
            //  CHECKING PASSWORD
            int len = (int)ret;
            char *space;
            char *delim;
            char *end;
            char *new;
            int index;
            int indexd;
            int indexe;
            int indnew;
            int validuser = 0;
            printf("Buffer First Char |%s|\n", &buffer[0]);
            space=strrchr(buffer,' ');
            index=(int)(space-buffer);
            sprintf(uspw, "%.*s", len, buffer+index);
            delim=strchr(uspw,'|');
            indexd=(int)(delim-uspw);
            sprintf(user, "%.*s",indexd-1,uspw+1);
            sprintf(pw, "%.*s",size(uspw),uspw+indexd+1);
            printf("User IS |*%s*|\n", user);
            printf("Password IS |*%s*|\n", pw);
            buffer[index]='\0';
            validuser = user_check(user, pw);
            if(validuser==0)
            {
              printf("The User isnt authorized to access this server\n");
                write(socketfd, "User Not Authorized\n",19);
            }

            if(validuser==1)
            {
              printf("Buffer after removing PW: |%s|\n", buffer);
              printf("A valid User is accessing this server\n");

                if(!strncmp(buffer,"GET ",4) || !strncmp(buffer,"get ",4) ) {
                    printf("A Get Was Accepted\n");
                    get(message,buffer, socketfd);
                }
                if(!strncmp(buffer,"list",4) || !strncmp(buffer,"LIST",4) ) {
                    printf("A list Was Accepted\n");

                        list(".",user,socketfd);
                }

                if(!strncmp(buffer,"put ",4) || !strncmp(buffer,"PUT ",4) ) {
                    printf("A put Was Accepted\n");
                    put(message, buffer, socketfd);
                }
             }
        }
      }
}

int main(int argc, char **argv)
{
    int listenfd = 0;
    int socketfd = 0;

    long port = strtol(argv[1], NULL, 10);
    struct sockaddr_in serv_addr;
    static struct sockaddr_in cli_addr;
    char sendBuff[1025];
    int numrv;
    socklen_t length;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 64) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    length = sizeof(cli_addr);
    while(socketfd = accept(listenfd, (struct sockaddr*)&cli_addr, &length))
    {
        //socketfd = accept(listenfd, (struct sockaddr*)&cli_addr, &length);
        printf("Conection Accepted\n");
        pthread_t sniffer_thread;
        int * new_sock = malloc(1);
        *new_sock = socketfd;

        if(pthread_create(&sniffer_thread, NULL, connection_handle, (void*) new_sock)< 0)
        {
            printf("Couldnt Create Thread\n");
            //return -1;
        }

        //close and sleep was here

    }
    close(socketfd);
    sleep(1);

    return 0;
}
