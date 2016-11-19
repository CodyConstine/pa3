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
#include <sys/stat.h>

#define BUFSIZE 8096
#define MAXBUFSIZE 100
char * rootdirectory;

int file_exist(const char *fname)
{
    FILE *file;
    if (file = fopen(fname, "r"))
    {
        fclose(file);
        return 1;
    }
    else
      return 0;
}
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
int checkdirectory(char * directory, char * root)
{
  DIR* dir;
  char fullroot[50];

    strcpy(fullroot,".");
    strcat(fullroot, root);
    strcat(fullroot,"/");
    strcat(fullroot,directory);
    dir = opendir(fullroot);
  //dir = opendir(directory);
//  puts("CheckinDirectory");
  if (dir)
  {
    puts("exists");
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
               checkdirectory(user,rootdirectory);//user/root
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
void readafile(int sock, char buf[], char * root)
{

    char filename[MAXBUFSIZE];
    FILE *f;
    int size=0;
    int nbytes;
    //char filereal[MAXBUFSIZE];
    printf("SIZE BEFORE %d\n", size);
    nbytes = read(sock, filename, MAXBUFSIZE);
    printf("Filereal: |%s|\n", filename);
    nbytes = read(sock, &size, sizeof(int));
    //nbytes = read(sock, size, MAXBUFSIZE);
    printf("SIZE %d\n", size);

    //memcpy(filename, buf+4, strlen(buf)+1);
    if(strlen(filename)!=0)
      {
        char fullfile[100];
        strcpy(fullfile,root);
        strcat(fullfile,"/");
        strcat(fullfile,filename);
        strcat(filename, "_put.s");
        printf("FILENAME %s\n", filename);
        bzero(buf,MAXBUFSIZE);
        //printf("zeroed");
        printf("BUFFER AFTER ZERO%s\n",buf);
        f = fopen(fullfile, "w+");
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



              bzero(buf,MAXBUFSIZE);

              nbytes = read(sock, buf, MAXBUFSIZE);
              if(strcmp(buf, "Done fetching file")==0)
              {
                printf("done\n");
                buf[0]='\0';
                break;
              }
              strcat(full, buf);
              //f=freopen(filename, "w+", stdout);
              printf("Sent this buffer |%s|\n",buf);

          }
          printf("%s PRINTED FULL",full);
          fwrite(full, 1, size, f);
          fclose(f);
        }

      }
}
void put(char msg[], char buf[], int sock, char * root)
{

  readafile(sock, buf, root);
  readafile(sock, buf, root);

}

int sendfile(char *filename, int sock)
{
  char buffer[MAXBUFSIZE];
  char message[MAXBUFSIZE];
  int nbytes=0;
  int size=0;
  FILE * f;
  printf("GETTING SIZE OF FILE %s\n", filename);
  f = fopen(filename, "r");

  if(f==NULL)
  {
    //sprintf(buffer, "File not found");
    //strcpy(message, buffer);
    //nbytes=write(sock,buffer, MAXBUFSIZE);
    //nbytes=write(sock,filename,MAXBUFSIZE);
    //nbytes=write(sock, &size, sizeof(int));
    printf("FILE NOT FOUND %s\n", filename);
    return -1;
  }
  else if (f!=NULL)
  {
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    printf("SIZE %d\n", size);
    nbytes=write(sock,filename,MAXBUFSIZE);
    nbytes=write(sock, &size, sizeof(int));
    printf("SENDING %s\n", filename);
    *buffer = '\0';
    while (fgets(buffer, MAXBUFSIZE, f)!=NULL)
    {
    //  printf("%s", buffer);
      nbytes=write(sock,buffer, MAXBUFSIZE);
    }

    sprintf(buffer, "Done fetching file");
  //  strcpy(message, buffer);

     nbytes=write(sock,buffer, MAXBUFSIZE);
     fclose(f);
     return 1;
  }

}

void get(char message[], char cmd[], int sock, char * root)
{
  /*
    with file given, add "." to front and look for ".1-4"im l
  */
  int nbytes;
  char filename[MAXBUFSIZE];
  char buffer[MAXBUFSIZE];
  FILE *f;

  memcpy(filename, cmd +4, strlen(cmd) +1);

  if(strlen(filename)!=0)
  {
    /*

      Given text.txt, search for .text.txt.1-4 and find ones on server
      then send those back.

    */
    char actualfile[30];
    char f1[30];
    char f2[30];
    char f3[30];
    char f4[30];
    strcpy(actualfile,".");
    strcat(actualfile,filename);
     char fullfile[100];
     strcpy(fullfile,root);
     strcat(fullfile,"/");
     strcat(fullfile,actualfile);
     strcpy(f1,fullfile);
     strcat(f1,".1");
     strcpy(f2,fullfile);
     strcat(f2,".2");
     strcpy(f3,fullfile);
     strcat(f3,".3");
     strcpy(f4,fullfile);
     strcat(f4,".4");
     int suc=0;
     printf("Looking For Files %s, %s, %s, %s\n",f1, f2, f3, f4);
     int filecheck = 0;
     if(!file_exist(f1))
     {
       filecheck+=1;
     }
     if(!file_exist(f2))
     {
       filecheck+=1;
     }
     if(!file_exist(f3))
     {
       filecheck+=1;
     }
     if(!file_exist(f4))
     {
       filecheck+=1;
     }
     if(filecheck==4)
     {
      // write(sock,"ABORT\0",6);
     }
     char checker[5];
     if(filecheck!=4)
     {
       //write(sock,"READY\0",6);


     }

     suc=sendfile(f1,sock);
     int failcheck=0;
     if(suc=-1)
     {
       printf("FILE %s Failed\n", f1);
       failcheck+=1;
     }
     suc=sendfile(f2,sock);
     if(suc=-1)
     {
       printf("FILE %s Failed\n", f2);
       failcheck+=1;
     }
     suc=sendfile(f3,sock);
     if(suc=-1)
     {
       printf("FILE %s Failed\n", f3);
       failcheck+=1;
     }
     suc=sendfile(f4,sock);
     if(suc=-1)
     {
       printf("FILE %s Failed\n", f4);
       failcheck+=1;
     }
     printf("FAILCHECK %d\n", failcheck);
     if(failcheck==4)
     {
       int sz = -1;
       sleep(1);
       write(sock,".\0",5);
       puts("SEND FAIL");
       sleep(1);
       write(sock,".\0",5);
       puts("SEND FAIL");


     }
     puts("out");

  }
}

void list(int sock, char * root)
{
    DIR *d;
    int nbytes;
    struct dirent *dir;
    char realdir[30];
    strcpy(realdir,root);
    strcat(realdir,"/");
    d = opendir(realdir);  //user
    char fulldir[30];
      if (d)
      {
        while ((dir = readdir(d)) != NULL)
        {
          printf("|%s|\n", dir->d_name);
          strcpy(fulldir,dir->d_name);
          int sizefl = size(fulldir);
          printf("SIZE OF File: %d\n",sizefl);
          fulldir[sizefl]='\0';
          printf("DIRECTORY IS |%s|\n",fulldir);


        //  nbytes= write(sock, &sizefl, sizeof(int));

          nbytes = write(sock, fulldir, MAXBUFSIZE);
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
       //printf("Initial Buffer |%s|\n", buffer);
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
              char fullroot[50];
              strcpy(fullroot,".");
              strcat(fullroot, rootdirectory);
              strcat(fullroot,"/");
              strcat(fullroot,user);
              printf("FULL ROOT PATH IS: %s\n", fullroot);
              printf("Buffer after removing PW: |%s|\n", buffer);
              printf("A valid User is accessing this server\n");

                if(!strncmp(buffer,"GET ",4) || !strncmp(buffer,"get ",4) ) {
                    printf("A Get Was Accepted\n");
                    get(message,buffer, socketfd, fullroot);
                }
                if(!strncmp(buffer,"list",4) || !strncmp(buffer,"LIST",4) ) {
                    printf("A list Was Accepted\n");

                        list(socketfd, fullroot);
                }

                if(!strncmp(buffer,"put ",4) || !strncmp(buffer,"PUT ",4) ) {
                    printf("A put Was Accepted\n");
                    put(message, buffer, socketfd, fullroot);
                }
             }
        }
      }
}

int main(int argc, char **argv)
{
    int listenfd = 0;
    int socketfd = 0;
    rootdirectory = argv[1];
    long port = strtol(argv[2], NULL, 10);
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
