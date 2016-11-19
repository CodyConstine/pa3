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
#include <openssl/md5.h>
#include <math.h>

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

void readafile(int sock, char buf[])
{
  int nbytes = 0;
  char filename[MAXBUFSIZE];
  char *space;
  FILE *f;
  int sizefl=0;
  printf("SIZE BEFORE %d\n", sizefl);
  nbytes = read(sock, filename, MAXBUFSIZE);
  printf("Filereal: |%s|\n", filename);
  nbytes = read(sock, &sizefl, sizeof(int));
  char *slash;
  int index;
  char filehold[MAXBUFSIZE];
  slash=strrchr(filename,'/');
  index=(int)(slash-filename);
  strncpy(filehold,filename+index+1,size(filename));
  printf("FILEHOLDER %s\n",filehold);
  //strcpy(filename,filehold);
  printf("SIZE %d\n", sizefl);
  //space=strrchr(buf,' ');
  //index=(int)(space-buf);
//  buf[index]='\0';
  //memcpy(filename, buf+4, strlen(buf)+1);

  if(strlen(filehold)!=0)
    {
      //strcat(filename,);
      //printf("FILENAME %s\n", filename);
      bzero(buf,MAXBUFSIZE);
      f = fopen(filehold, "w");


      char full[sizefl];
      bzero(full, sizefl);
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
              buf[0]='\0';
              break;
            }
            strcat(full, buf);
        }
        //printf("%s PRINTED FULL",full);
        fwrite(full, 1, sizefl, f);
        fclose(f);

      }

    }
}

void get (char buf[], char filename[], int sock, int sock2)//add sockets
{
    readafile(sock, buf);
    readafile(sock, buf);
    readafile(sock2, buf);
    readafile(sock2, buf);
    //gotta read from each socket
    printf("GETGETGETFILE: %s\n", filename);
    char getcommand[60];
    strcpy(getcommand,"python get.py ");
    strcat(getcommand, filename);
    system(getcommand);

}
int splitfile(char * filename)
{
  FILE * f;
  f = fopen(filename, "r");
  int size;
  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);
   /*
     HERE add ability to split file into 4 pieces and ship it all
   */
   float filebyfour = size/4;
     printf("File size Divide = %f", filebyfour);
   int lastpart = -1;
   if(floor(filebyfour) != filebyfour)
   {
     filebyfour=floor(filebyfour);
     lastpart=size-(filebyfour*3);
     printf("Last File Size = %d\n", lastpart);
   }


   int splitsize=(int)filebyfour;
   char file1[splitsize];
   char file2[splitsize];
   char file3[splitsize];
   char file4[splitsize];

   //split file. First 3 by filebyfour value, last file by lastpart value
   for(int i=0; i<splitsize; i++)
   {
         file1[i]=getc(f);
   }
   for(int i=0; i<splitsize; i++)
   {
         file2[i]=getc(f);
   }
   for(int i=0; i<splitsize; i++)
   {
         file3[i]=getc(f);
   }
   for(int i=0; i<(size-(splitsize*3)); i++)
   {
         file4[i]=getc(f);
   }
   file1[splitsize]='\0';
   file2[splitsize]='\0';
   file3[splitsize]='\0';
   file4[size-(splitsize*3)]='\0';
   printf("|%s|", file1);
   printf("|%s|", file2);
   printf("|%s|", file3);
   printf("|%s|", file4);

  FILE * f1;
  FILE * f2;
  FILE * f3;
  FILE * f4;

  char newfile[50];
  char file1nm[50];
  char file2nm[50];
  char file3nm[50];
  char file4nm[50];

  strcpy(newfile,".");
  strcat(newfile,filename);
  strcpy(file1nm, newfile);
  strcpy(file2nm, newfile);
  strcpy(file3nm, newfile);
  strcpy(file4nm, newfile);
  strcat(file1nm, ".1");
  strcat(file2nm, ".2");
  strcat(file3nm, ".3");
  strcat(file4nm, ".4");

  printf("FileNamenew: |%s|\n",file1nm);

  f1 = fopen(file1nm, "w");
  fwrite(file1, 1, splitsize, f1);
  fclose(f1);
  f2 = fopen(file2nm, "w");
  fwrite(file2, 1, splitsize, f2);
  fclose(f2);
  f3 = fopen(file3nm, "w");
  fwrite(file3, 1, splitsize, f3);
  fclose(f3);
  f4 = fopen(file4nm, "w");
  fwrite(file4, 1, size-(splitsize*3), f4);
  fclose(f4);
  //FILES ARE CORRECT
}
void sendtoserver(char * filename, int sock)
{
      char buffer[MAXBUFSIZE];
      int nbytes;
      FILE *f;
      f = fopen(filename, "r");
      int size=0;
      fseek(f, 0, SEEK_END);
      size = ftell(f);
      fseek(f, 0, SEEK_SET);
      printf("SIZE %d\n", size);
       nbytes=write(sock, filename, MAXBUFSIZE);
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
           printf("SENT THIS Buffer%s\n", buffer);
           nbytes=write(sock,buffer, MAXBUFSIZE);
         }

         sprintf(buffer, "Done fetching file");
        //  strcpy(message, buffer);

          nbytes=write(sock,buffer, MAXBUFSIZE);
       }
       fclose(f);
       char del1[45];
       strcpy(del1,"rm -rf ");
       strcat(del1,filename);
       printf("DEL FILE %s\n",del1);
       system(del1);
}
void put(char cmd[], int sock, int sock2)//add sockets
{

  char *space;
  int index;
	char filename[MAXBUFSIZE];
  puts(cmd);
  //puts("Before Memcpy\n");
  space=strrchr(cmd,' ');
  index=(int)(space-cmd);
  cmd[index]='\0';
  puts(cmd);
	memcpy(filename, cmd +4, strlen(cmd) +1);
  puts(filename);
	if(strlen(filename)!=0)
	{

     char newfile[50];
     char file1nm[50];
     char file2nm[50];
     char file3nm[50];
     char file4nm[50];

     strcpy(newfile,".");
     strcat(newfile,filename);
     strcpy(file1nm, newfile);
     strcpy(file2nm, newfile);
     strcpy(file3nm, newfile);
     strcpy(file4nm, newfile);
     strcat(file1nm, ".1");
     strcat(file2nm, ".2");
     strcat(file3nm, ".3");
     strcat(file4nm, ".4");


     int returnval = splitfile(filename);

    //send files, then remove files

    sendtoserver(file1nm, sock);
    sendtoserver(file4nm, sock);
    sendtoserver(file2nm, sock2);
    sendtoserver(file3nm, sock2);





	}

}



void list(char * root, char * user, int sock, int sock2)
{

  //DUMP ALL LISTING INTO A FILE
  //GO THROUGH FILE, if NEW> add, ELSE CHECK FOR COMPL
  //Print results
  FILE * f;
  int nbytes = 0;
  char buffer[MAXBUFSIZE];
  char buffer2[MAXBUFSIZE];
  int sizefl=0;
  while(1)
  {
  //  nbytes = read(sock, &sizefl, sizeof(int));
  //  printf("SIZE OF READ: %d\n",sizefl);
    nbytes = read(sock, buffer, MAXBUFSIZE);
    nbytes = read(sock2, buffer2, MAXBUFSIZE);
    //printf("bytes accepted %d\n", nbytes);
    //printf("bytes received %d\n", nbytes);
    if(strcmp(buffer,"Done With LS")==0 && strcmp(buffer2,"Done With LS")==0 )
    {
      break;
    }
    //printf("BUFFER IS %s\n", buffer);
    //printf("BUFFER2 IS %s\n", buffer2);
    /*
    CATCH ALL INTO ARRAY

    */
    int ret;
    f = fopen("List.txt","a+");
    ret = fwrite(buffer, 1, size(buffer), f);
    if(ret<0)
    {
      printf("BUFFER NOT WRITTEN %s\n", buffer);
    }
    fwrite("\n",1,1,f);
    ret = fwrite(buffer2, 1, size(buffer2), f);
    if(ret<0)
    {
      printf("BUFFER2 NOT WRITTEN %s\n", buffer2);
    }
    fwrite("\n",1,1,f);
    fclose(f);
    ///DELETE LIST.TXT AT END!!!
  }
  system("python ./listmanage.py");
  FILE * fi = fopen("listout.txt","r");
  char buf[MAXBUFSIZE];
  while(fgets(buf, MAXBUFSIZE, fi))
    {
      printf("%s",buf);
    }

    system("rm -rf List.txt");
    system("rm -rf listout.txt");


//printf("clear\n");
}


int main(void)
{
    int sockfd1 = 0;
    int sockfd2, sockfd3, sockfd4=0;
    int bytesReceived = 0;
    int nbytes = 0;
    int nbytes2 = 0;
    char buffer[MAXBUFSIZE];
    char buffer2[MAXBUFSIZE];
    char *newline = NULL;

    struct sockaddr_in serv_addr1;
    struct sockaddr_in serv_addr2;
    struct sockaddr_in serv_addr3;
    struct sockaddr_in serv_addr4;
    config();
    /* Create a socket first */
    //CREATE ALL sockets



    if((sockfd1 = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    if((sockfd2 = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    /* Initialize sockaddr_in data structure */
    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_port = htons(D1P); // port
    serv_addr1.sin_addr.s_addr = inet_addr("127.0.0.1");

    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(D2P); // port
    serv_addr2.sin_addr.s_addr = inet_addr("127.0.0.1");
    /* Attempt a connection */
    //connect to all servers




    if(connect(sockfd1, (struct sockaddr *)&serv_addr1, sizeof(serv_addr1))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }
    if(connect(sockfd2, (struct sockaddr *)&serv_addr2, sizeof(serv_addr2))<0)
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
        //ADD SOCKETS
        nbytes = write(sockfd1, command, MAXBUFSIZE);

        nbytes = write(sockfd2, command, MAXBUFSIZE);
        if(nbytes<0)

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

      //printf("%s\n",cmd);
      //printf("|%s|\n",lstcmd);
      if(strcmp(lstcmd, "list")==0 || strcmp(cmd, "put")==0 || strcmp(cmd, "get")==0)
      {
        strcat(command," ");
        strcat(command, uspw);
        //printf("%s\n",command);
        //ADD sockets
        char flnm[MAXBUFSIZE];
        memcpy(flnm, command+4, strlen(command)+1);
        //printf("FILENMMAIN: %s", flnm);
        char * spc;
        int in;
        spc = strrchr(flnm,' ');
        in = (int)(spc-flnm);
        flnm[in]='\0';
        //printf("FILENMMAINDEL: %s", flnm);

        nbytes2 = write(sockfd2, command, MAXBUFSIZE);

        nbytes = write(sockfd1, command, MAXBUFSIZE);



        if(strcmp(cmd, "get")==0)
        {
          get(command, flnm, sockfd1, sockfd2);
        }
        if(strcmp(cmd, "put")==0)
        {
          put(command, sockfd1, sockfd2);
        }
        if(strcmp(lstcmd, "list")==0)
        {
          //printf("Files in your directory:\n");
          list("root", "user", sockfd1, sockfd2);
          //list("root", "user", sockfd2);
        }
        //printf("clear\n");
      }
      else
        printf("%s\n", command);




    }
    close(sockfd1);
    close(sockfd2);


    return 0;
}
