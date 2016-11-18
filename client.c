#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define MAXBUFSIZE 100
#define BUFSIZE 8096

int D1P = -1;
int D2P = -1;
int D3P = -1;
int D4P = -1;
char user[50];
char pw[50];


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

int config(void)
{
  FILE * fi;
  char * space;
  char * colon;
  int indexs;
  int indexc;
  int num=0;
  char D1[50];
  char D2[50];
  char D3[50];
  char D4[50];
  char * filename = "dfc.conf";
  fi = fopen( filename , "r");
//  printf("ERROR\n");
  char str[1024];


    if (fi) {
      while (fgets(str, 1024, fi))
      {   //DFS1 127.0.0.1:10001
        //  printf("ERROR2\n");
          printf("%s\n",str);
          //printf("ERROR3\n");
          for(int i=0;i<size(str);i++)  /* remove CF and LF characters */
            if(str[i] == '\r' || str[i] == '\n')
              str[i]='\0';
          if(num==0){//server 1
            space=strchr(str,' ');
            indexs=(int)(space-str);
            colon=strchr(str,':');
            indexc=(int)(colon-str);
            sprintf(D1, "%.*s", size(str), str+indexc+1);
            printf("Port 1 |%s|\n", D1);
            /*
              rootdir = str[0:indexs]
              ip = str[indexs:indexc]
              port = str[indexc:end]
            */
          }
          if(num==1){//server 2
            space=strchr(str,' ');
            indexs=(int)(space-str);
            colon=strchr(str,':');
            indexc=(int)(colon-str);
            sprintf(D2, "%.*s", size(str), str+indexc+1);
            printf("Port 2 |%s|\n", D2);

          }
          if(num==2){//server 3
            space=strchr(str,' ');
            indexs=(int)(space-str);
            colon=strchr(str,':');
            indexc=(int)(colon-str);
            sprintf(D3, "%.*s", size(str), str+indexc+1);
            printf("Port 3 |%s|\n", D3);

          }
          if(num==3){//server 4
            space=strchr(str,' ');
            indexs=(int)(space-str);
            colon=strchr(str,':');
            indexc=(int)(colon-str);
            sprintf(D4, "%.*s", size(str), str+indexc+1);
            printf("Port 4 |%s|\n", D4);

          }
          if(num==4){//username  Username: Jim
            space=strchr(str,' ');
            indexs=(int)(space-str);
            //user = str[index:strlen(str)]
            sprintf(user, "%.*s", size(str), str+indexs+1);
            printf("User: |%s|\n", user);
          }
          if(num==5){//password  Password: KillMyself123
            space=strchr(str,' ');
            indexs=(int)(space-str);
            sprintf(pw, "%.*s", size(str), str+indexs+1);
            printf("Password |%s|\n", pw);
          //  pw = str[index:strlen(str)]
          }
          num+=1;
      }
      fclose(fi);
      sscanf(D1, "%d", &D1P);
      sscanf(D2, "%d", &D2P);
      sscanf(D3, "%d", &D3P);
      sscanf(D4, "%d", &D4P);
  }

}

void get (char buf[], int sock)
{
	int nbytes = 0;
	char filename[MAXBUFSIZE];
  char *space;
  int index;
	FILE *f;
	int size;
	nbytes = read(sock, &size, sizeof(int));
	printf("SIZE %d\n", size);
  space=strrchr(buf,' ');
  index=(int)(space-buf);
  buf[index]='\0';
	memcpy(filename, buf+4, strlen(buf)+1);
	if(strlen(filename)!=0)
		{
			strcat(filename, "_get.c");
			//printf("FILENAME %s\n", filename);
			bzero(buf,MAXBUFSIZE);
			f = fopen(filename, "w");


			char full[size];
			bzero(full, size);
			if(nbytes<0)
			{
				printf("ERROR GETTING FILE\n");
			}
			if (strcmp(buf, "File not found" ) != 0)
			{
				while(1)
				{
						bzero(buf,MAXBUFSIZE);

						nbytes = read(sock, buf, MAXBUFSIZE);
						if(strcmp(buf, "Done fetching file")==0)
						{
							printf("done\n");
							break;
						}
						strcat(full, buf);
				}
				//printf("%s PRINTED FULL",full);
				fwrite(full, 1, size, f);
				fclose(f);
			}

		}

}
void put(char cmd[], int sock)
{
	int nbytes;
  char *space;
  int index;
	char filename[MAXBUFSIZE];
	char buffer[MAXBUFSIZE];
	FILE *f;
  puts(cmd);
  puts("Before Memcpy\n");
  space=strrchr(cmd,' ');
  index=(int)(space-cmd);
  cmd[index]='\0';
  puts(cmd);
	memcpy(filename, cmd +4, strlen(cmd) +1);
  puts(filename);
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
			 //strcpy(message, buffer);
			 nbytes=write(sock,buffer, MAXBUFSIZE);
		 }
		 else
		 {
			 *buffer = '\0';
			 while (fgets(buffer, MAXBUFSIZE, f)!=NULL)
			 {
				 //printf("%s", buffer);
				 nbytes=write(sock,buffer, MAXBUFSIZE);
			 }

			 sprintf(buffer, "Done fetching file");
			//  strcpy(message, buffer);

			  nbytes=write(sock,buffer, MAXBUFSIZE);
		 }
	}

}

void list(char * root, char * user, int sock)
{
  int nbytes = 0;
  char buffer[MAXBUFSIZE];
  while(1)
  {
    nbytes = read(sock, buffer, MAXBUFSIZE);
    //printf("bytes accepted %d\n", nbytes);
    if(strcmp(buffer,"Done With LS")==0)
    {
      break;
    }
    printf("%s\n", buffer);
    buffer[0]='\0';
  }
//printf("clear\n");
}


int main(void)
{
    int sockfd1 = 0;
    int sockfd2, sockfd3, sockfd4=0;
    int bytesReceived = 0;
    int nbytes = 0;
    char buffer[MAXBUFSIZE];
    char *newline = NULL;

    struct sockaddr_in serv_addr1;
    config();
    /* Create a socket first */
    if((sockfd1 = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    /* Initialize sockaddr_in data structure */
    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_port = htons(D1P); // port
    serv_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");

    /* Attempt a connection */
    if(connect(sockfd1, (struct sockaddr *)&serv_addr1, sizeof(serv_addr1))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    char command[MAXBUFSIZE];
    while(1)
    {
      *command = '\0';
      printf("> ");

      fgets( command, MAXBUFSIZE, stdin);
      newline = strchr(command, '\n'); // get rid of newline character
      if(newline != NULL) *newline = '\0';
      if(strcmp(command, "exit")==0)
      {
        nbytes = write(sockfd1, command, MAXBUFSIZE);
        break;
      }
      char uspw[100];
      strcpy(uspw,user);
      strcat(uspw,"|");
      strcat(uspw,pw);
      //puts(uspw);
      char cmd[4];
      strncpy(cmd,command,3);
      cmd[3]=0;
      char lstcmd[5];
      strncpy(lstcmd,command,4);
      lstcmd[4]=0;

      printf("%s\n",cmd);
      printf("|%s|\n",lstcmd);
      if(strcmp(lstcmd, "list")==0 || strcmp(cmd, "put")==0 || strcmp(cmd, "get")==0)
      {
        strcat(command," ");
        strcat(command, uspw);
        printf("%s\n",command);
        nbytes = write(sockfd1, command, MAXBUFSIZE);
        if(strcmp(cmd, "get")==0)
        {
          get(command, sockfd1);
        }
        if(strcmp(cmd, "put")==0)
        {
          put(command, sockfd1);
        }
        if(strcmp(lstcmd, "list")==0)
        {
          printf("list accepted %s\n",command);
          list("root", "user", sockfd1);
        }
        //printf("clear\n");
      }
      else
        printf("%s\n", command);


      // Blocks till bytes are received
      struct sockaddr_in from_addr;
      int addr_length = sizeof(struct sockaddr);
      bzero(buffer,sizeof(buffer));
      if(strcmp(command, "ls")==0 || strcmp(command, "put")==0 || strcmp(command, "get")==0)
      {
        nbytes = read(sockfd1, buffer, MAXBUFSIZE);
        printf("Server says %s\n", buffer);
        //printf("clear\n");
      }

    }
    close(sockfd1);


    return 0;
}
