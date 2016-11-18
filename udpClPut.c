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

#define MAXBUFSIZE 100
// Linux IP 127.0.1.1
/* You will have to modify the program below */
//deal with get
void get(char buf[], int sock, struct sockaddr_in remote);
//deal with put
void put(char cmnd[], int sock, struct sockaddr_in remote);


int main (int argc, char * argv[])
{

	int nbytes;                             // number of bytes send by sendto()
	int sock;                               //this will be our socket
	char buffer[MAXBUFSIZE];
	char *newline = NULL;  //to remove new
	struct sockaddr_in remote;              //"Internet socket address structure"

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	/******************
	  Here we populate a sockaddr_in struct with
	  information regarding where we'd like to send our packet
	  i.e the Server.
	 ******************/
	bzero(&remote,sizeof(remote));               //zero the struct
	remote.sin_family = AF_INET;                 //address family
	remote.sin_port = htons(atoi(argv[2]));      //sets port to network byte order
	remote.sin_addr.s_addr = inet_addr(argv[1]); //sets remote IP address

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}
	unsigned int remote_length = sizeof(remote);
	/******************
	  sendto() sends immediately.
	  it will report an error if the message fails to leave the computer
	  however, with UDP, there is no error if the message is lost in the network once it leaves the computer.
	 ******************/
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
			nbytes = sendto(sock, command, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
			break;
		}

		char cmd[4];
		strncpy(cmd,command,3);
		cmd[3]=0;
		//printf("%s",cmd);
		if(strcmp(command, "ls")==0 || strcmp(cmd, "put")==0 || strcmp(cmd, "get")==0)
		{
			nbytes = sendto(sock, command, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
			if(strcmp(cmd, "get")==0)
			{
				get(command, sock, remote);
			}
			if(strcmp(cmd, "put")==0)
			{
				put(command, sock, remote);
			}
			if(strcmp(command, "ls")==0)
			{
				while(1)
				{
					nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, &remote_length);
					if(strcmp(buffer,"Done With LS")==0)
					{
						break;
					}
					printf("%s", buffer);
				}
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
			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, &remote_length);
			printf("Server says %s\n", buffer);
			//printf("clear\n");
		}

	}
	close(sock);

}


void get (char buf[], int sock, struct sockaddr_in remote)
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
void put(char cmd[], int sock, struct sockaddr_in remote)
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
			 //strcpy(message, buffer);
			 nbytes=sendto(sock,buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
		 }
		 else
		 {
			 *buffer = '\0';
			 while (fgets(buffer, MAXBUFSIZE, f)!=NULL)
			 {
				 //printf("%s", buffer);
				 nbytes=sendto(sock,buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
			 }

			 sprintf(buffer, "Done fetching file");
			//  strcpy(message, buffer);

			  nbytes=sendto(sock,buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
		 }
	}

}
