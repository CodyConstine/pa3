#include <sys/types.h>
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
#include <string.h>
//for ls
#include <dirent.h>

/* You will have to modify the program below */

#define MAXBUFSIZE 100
//a function to handle get
void get(char cmnd[], char msg[], int sock, struct sockaddr_in remote);
//a function to handle put
void put(char cmnd[], char msg[], int sock, struct sockaddr_in remote);
int main (int argc, char * argv[] )
{
	printf("Waiting for Client\n");

	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	char message[MAXBUFSIZE];
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	//printf("Creating Socket");
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}

	//printf("Binding");
	/******************
	  Once we've created a socket, we must bind that socket to the
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}
	//printf("socket bound");
	while(1) {
		printf("> ");
		remote_length = sizeof(remote);

		//waits for an incoming message
		bzero(buffer,sizeof(buffer));
		//printf("zeroed");
		nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, &remote_length);
		char cmd[4];
		strncpy(cmd,buffer,3);
		cmd[3]=0;
		if(strcmp(cmd, "get")==0)
		{
			get(message, buffer, sock, remote);
		}
		if(strcmp(cmd, "put")==0)
		{
			put(message, buffer, sock, remote);
		}
		if(strcmp(cmd, "ls")==0)
		{
			DIR           *d;
 			struct dirent *dir;
 			d = opendir(".");
 			if (d)
 			{
	 			while ((dir = readdir(d)) != NULL)
	 			{
		 			printf("%s\n", dir->d_name);

					nbytes = sendto(sock, dir->d_name, nbytes, 0, (struct sockaddr *) &remote, sizeof(remote));
	 			}
					char donesig[]="Done With LS";
					nbytes = sendto(sock, donesig, nbytes, 0, (struct sockaddr *) &remote, sizeof(remote));
	 			closedir(d);
 				}

		}
		printf("The client says %s\n", buffer);
		if(strcmp(buffer, "exit")==0)
		{
			break;
		}
		char msg[] = "SERVER RESPONSE";
		nbytes = sendto(sock, msg, nbytes, 0, (struct sockaddr *) &remote, sizeof(remote));
	}
	close(sock);
}


void get(char message[], char cmd[], int sock, struct sockaddr_in remote)
{
	int nbytes;
	char filename[MAXBUFSIZE];
	unsigned int remote_length;
	remote_length = sizeof(remote);
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

		 nbytes=sendto(sock, &size, sizeof(int), 0, (struct sockaddr *) &remote, sizeof(remote));
		 if(f==NULL)
		 {
			 sprintf(buffer, "File not found");
			 strcpy(message, buffer);
			 nbytes=sendto(sock,buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
		 }
		 else
		 {
			 *buffer = '\0';
			 while (fgets(buffer, MAXBUFSIZE, f)!=NULL)
			 {
				 printf("%s", buffer);
				 nbytes=sendto(sock,buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
			 }

			 sprintf(buffer, "Done fetching file");
			 strcpy(message, buffer);

			  nbytes=sendto(sock,buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
		 }
	}

}
void put(char msg[], char buf[], int sock, struct sockaddr_in remote)
{
	int nbytes;
	char filename[MAXBUFSIZE];
	unsigned int remote_length;
	remote_length = sizeof(remote);
	FILE *f;
	int size;
	nbytes = recvfrom(sock, &size, sizeof(int), 0, (struct sockaddr *) &remote, &remote_length);
	printf("SIZE %d\n", size);
	memcpy(filename, buf+4, strlen(buf)+1);
	if(strlen(filename)!=0)
		{
			strcat(filename, "_server");
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

						nbytes = recvfrom(sock, buf, MAXBUFSIZE, 0, (struct sockaddr *) &remote, &remote_length);
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
