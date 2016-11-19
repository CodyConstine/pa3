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
#include <stdio.h>
#include <openssl/md5.h>
#include <ctype.h>


#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define PATH_LEN 256
#define MD5_LEN 32



#define MAXBUFSIZE 1024

//thread handlers
int listHandler(char *);
int getHandler(char*, char *);
int putHandler(char*, char *);

//file parser functions
char * getUser(char *);
char * getPassword(char *);
int getServerPort(int, char *);
int getMD5(char *, char *);

int combindFiles(char*);
int fileIndex(char*);

void readafile(int);


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
            int responce = putHandler(pch, argv[1]);
            printf("Put Handler returned: %d\n", responce);
		}
		else if(strncmp(pch, "get", 3) == 0)
		{
            pch = strtok (NULL, " ");
            int responce = getHandler(pch, argv[1]);
            printf("Get Handler returned: %d\n", responce);
		}
		else if(strncmp(pch, "list", 2) == 0)
		{
            int responce = listHandler(argv[1]);
            printf("List Handler returned: %d\n", responce);
		}
		else
			printf("%s is an invalid command.\n", command);

	}
    return 0;
}

int listHandler(char * conf)
{
    printf("LIST command recieved.\n");
    return 0;
}
int getHandler(char * file, char * conf)
{
    int sockfd, portno;
    printf("Get command recieved: %s\n", file);


    struct sockaddr_in serv_addr;
    struct hostent *server;
    int i;
    for(i = 0; i<4; i++)
    {
        portno = getServerPort(i,conf);
        char * user = getUser(conf);
        char * password = getPassword(conf);
        printf("userName: %s password: %s\n", user, password);
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
        {
            printf("ERROR opening socket\n");
        }
        server = gethostbyname("127.0.0.1");
        if (server == NULL)
        {
            fprintf(stderr,"ERROR, no such host\n");
            exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
        serv_addr.sin_port = htons(portno);

        if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
        {
            printf("ERROR connecting\n");
        }
        char * message = (char*)malloc(strlen("get ")+strlen(file)+1);
        strcpy(message, "get ");
        strcat(message, file);
        printf("Sending file message: %s\n", message);
        char * finalMessage = (char*)malloc(strlen(message)+strlen(user)+strlen(password)+strlen(",,")+1);
        strcpy(finalMessage,message);
        strcat(finalMessage, ",");
        strcat(finalMessage, user);
        strcat(finalMessage,",");
        strcat(finalMessage,password);
        send(sockfd, finalMessage, strlen(finalMessage), 0);

        //read_size = recv()
        readafile(sockfd);
        readafile(sockfd);
        close(sockfd);
    }
    if(combindFiles(file)==-1){
        printf("The file:%s cannot be recreated.\n", file);
    }
    else
    {
        printf("File Successfully retrieved.\n");
    }
    return 0;
}
int putHandler(char * file, char * conf)
{
    int sockfd, portno;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    unsigned long long int num;

    char md5[MD5_LEN + 1];
    if (!getMD5(file, md5)) {
        puts("Error occured!");
        exit(0);
    } else {
        sscanf(md5, "%llx", &num);  // assuming you checked input
        //printf("Success! MD5 sum is: %s\n", md5);
        //printf("Success! MD5 sum is: %llu\n", num);
    }

    num = num%4;
    //printf("%llu\n", num);

    //char buffer[256];
    portno = getServerPort(1,conf);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("ERROR opening socket\n");
    }
    server = gethostbyname("127.0.0.1");
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("ERROR connecting\n");
    }

    printf("PUT command recieved: %s\n", file);
    char * message = (char*)malloc(strlen("put ")+strlen(file)+1);
    strcpy(message, "put ");
    strcat(message, file);
    printf("Sending file message: %s\n", message);
    send(sockfd, message, strlen(message), 0);
    close(sockfd);
    return 0;
}


int getServerPort(int index, char * conf)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * pch;

    char * root;

    file = fopen(conf, "r");
    if (file == NULL)
    {
        printf("%s is missing\n", conf);
        exit(EXIT_FAILURE);
    }
    int i =0;
    while ((read = getline(&line, &len, file)) != -1) {
        pch = strtok (line,":");//get indetinfer
        if(strncmp(pch, "#", 1) == 0)//any line with # will be filtered out
        {
            continue;
        }
        if(i==index) //all others will work the same
        {
            pch = strtok (NULL, ":"); //grab the value
            printf("Port: %s", pch);
            root = (char*)malloc(strlen(pch)+1); //create memory
            strcpy(root,pch); //copy value
            root[strcspn(root, "\n")] = 0; //remove /n
            break; //next line
        }
        i++;
    }
    printf("port: %s\n", root);
    fclose(file);
    if (line){
       free(line);
    }
    return atoi(root);
}
int getMD5(char * file, char * md5_sum)
{
    #define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
    char cmd[PATH_LEN + sizeof (MD5SUM_CMD_FMT)];
    sprintf(cmd, MD5SUM_CMD_FMT, file);
    #undef MD5SUM_CMD_FMT

    FILE *p = popen(cmd, "r");
    if (p == NULL) return 0;

    int i, ch;
    for (i = 0; i < MD5_LEN && isxdigit(ch = fgetc(p)); i++) {
        *md5_sum++ = ch;
    }

    *md5_sum = '\0';
    pclose(p);
    return i == MD5_LEN;
}
char * getUser(char * conf)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * pch;

    char * root;

    file = fopen(conf, "r");
    if (file == NULL)
    {
        printf("%s is missing\n", conf);
        exit(EXIT_FAILURE);
    }
    int i =0;
    while ((read = getline(&line, &len, file)) != -1) {
        pch = strtok (line,":");//get indetinfer
        if(strncmp(pch, "#", 1) == 0)//any line with # will be filtered out
        {
            continue;
        }
        if(strcmp("Username", pch)==0) //all others will work the same
        {
            pch = strtok (NULL, ":"); //grab the value
            //printf("Port: %s", pch);
            root = (char*)malloc(strlen(pch)+1); //create memory
            strcpy(root,pch); //copy value
            root[strcspn(root, "\n")] = 0; //remove /n
            break; //next line
        }
        i++;
    }
    printf("port: %s\n", root);
    fclose(file);
    if (line){
       free(line);
    }
    return root;
}
char * getPassword(char * conf)
{
    FILE * file;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    char * pch;

    char * root;

    file = fopen(conf, "r");
    if (file == NULL)
    {
        printf("%s is missing\n", conf);
        exit(EXIT_FAILURE);
    }
    int i =0;
    while ((read = getline(&line, &len, file)) != -1) {
        pch = strtok (line,":");//get indetinfer
        if(strncmp(pch, "#", 1) == 0)//any line with # will be filtered out
        {
            continue;
        }
        if(strcmp("Password", pch)==0) //all others will work the same
        {
            pch = strtok (NULL, ":"); //grab the value
            //printf("Port: %s", pch);
            root = (char*)malloc(strlen(pch)+1); //create memory
            strcpy(root,pch); //copy value
            root[strcspn(root, "\n")] = 0; //remove /n
            break; //next line
        }
        i++;
    }
    printf("port: %s\n", root);
    fclose(file);
    if (line){
       free(line);
    }
    return root;
}

int combindFiles(char * file)
{
    //char directory[MAXBUFSIZE];
    int index[4];
    int j = 0;
    for(j = 0; j<4; j++)
    {
        index[j] = 0;
    }
	printf("Reading local files\n");
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
    char * testName = (char*)malloc(strlen(file)+strlen(".")+1);
    strcpy(testName, ".");
    strcat(testName, file);
    int i = 0;
	if (d)
	{
		while ((dir = readdir(d)) != NULL)
        {
            if(strncmp(dir->d_name,testName,strlen(testName))==0)
            {
                //printf("File name: %s\n", dir->d_name);
                int r = fileIndex(dir->d_name);
                index[r-1] = 1;
                i++;
            }
			//strcat(directory, dir->d_name);
			//strcat(directory, ",");
        }
		closedir(d);
	}
    int returnStatus = 0;
    for(i = 0; i<4; i++)
    {
        //printf("Index[i] has: %d\n", index[i]);
        if(index[i]!=1)
        {
            printf("%d File is missing, incomplete file will result.\n", i+1);
            returnStatus = -1;
        }
    }
    if(returnStatus == -1)
    {
        return returnStatus;
    }

    FILE *pFile;
    FILE *pFile2;
    char buffer[256];

    remove(file);
    pFile2=fopen(file, "a");
    for(i =0; i< 4; i++) {
        char * tempFile = (char*)malloc(strlen(file)+strlen(".")+strlen(".1")+1);
        strcpy(tempFile, ".");
        strcat(tempFile, file);
        strcat(tempFile, ".");
        //printf("Tempfile name: %s\n", tempFile);
        char temp[1];
        sprintf(temp,"%d",i+1);
        strcat(tempFile, temp);
        //printf("Tempfile name: %s\n", tempFile);
        pFile=fopen(tempFile, "r");
        while(!feof(pFile))
        {
            if(fgets(buffer, 100, pFile) != NULL)
            {
            fseek(pFile2, -100, SEEK_END);
            fprintf(pFile2, buffer);
            }
        }
        fclose(pFile);
        remove(tempFile);
        free(tempFile);
    }

    fclose(pFile2);

    return returnStatus;
}

int fileIndex(char * file)
{
    char * pch;
    char * fileExtension;
    pch = strtok(file, ".");
    while(pch != NULL) {//this finds the extension
        fileExtension = pch;
        pch = strtok(NULL, ".");
    }
    return atoi(fileExtension);
}

void readafile(int sock)
{
    char buf[MAXBUFSIZE];
    int nbytes = 0;
    char filename[MAXBUFSIZE];
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
    strncpy(filehold,filename+index+1,strlen(filename));
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
