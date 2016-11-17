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


int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        printf("Correct Usage: ./dfs <working directory> <port>\n");
    }
    printf("Waiting for Client\n");

	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[2]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		printf("unable to create socket");
	}
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
		printf("The client says %s\n", buffer);
		if(strcmp(buffer, "exit")==0)
		{
			exit(0);
		}
		else if(strncmp(buffer, "put", 3)==0)
		{
			nbytes = sendto(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
			//Read Picture Size
			printf("Reading File Size\n");
			int size;
			nbytes = recvfrom(sock, &size, sizeof(int), 0, (struct sockaddr *) &remote, &remote_length);
			//Read Picture Byte Array
			printf("Reading File Byte Array\n");
			char p_array[size];
			nbytes = recvfrom(sock, &p_array, size, 0, (struct sockaddr *) &remote, &remote_length);

			//Convert it Back into Picture
			printf("Converting Byte Array to File\n");
			FILE *image;
			int i = 0;
			char name[MAXBUFSIZE];
			for(i =0;i<MAXBUFSIZE-3;i++)
			{
					name[i]=buffer[i+3];
			}
			fprintf(stderr, "Creating File %s\n", name);
			image = fopen(name, "w");
			fwrite(p_array, 1, sizeof(p_array), image);
			fclose(image);
		}
		else if(strncmp(buffer, "get", 3)==0)
		{
			fprintf(stderr, "%s\n", buffer);
			printf("Getting File Size\n");
			FILE *picture;
			int i = 0;
			char name[MAXBUFSIZE];
			for(i =0;i<MAXBUFSIZE-3;i++)
			{
					name[i]=buffer[i+3];
			}
			picture = fopen(name, "r");
			printf("Getting File Name: %s\n", name);
			int size;
			fseek(picture, 0, SEEK_END);
			size = ftell(picture);
			fseek(picture, 0, SEEK_SET);
			printf("Client says %s\n", buffer);
			//Send Picture Size
			printf("Sending File Size\n");
			nbytes = sendto(sock, &size, sizeof(size), 0, (struct sockaddr *) &remote, sizeof(remote));
			//Send Picture as Byte Array
			printf("Sending File as Byte Array\n");
			char send_buffer[size];
			while(!feof(picture)) {
			    fread(send_buffer, 1, sizeof(send_buffer), picture);
					nbytes = sendto(sock, send_buffer, sizeof(send_buffer), 0, (struct sockaddr *) &remote, sizeof(remote));
			    bzero(send_buffer, sizeof(send_buffer));
			}
		}
		else if(strcmp(buffer, "ls")==0)
		{
			char directory[MAXBUFSIZE];
			printf("Reading local files\n");
			DIR *d;
			struct dirent *dir;
			d = opendir(".");
			if (d)
			{
				while ((dir = readdir(d)) != NULL)
		    {
					strcat(directory, dir->d_name);
					strcat(directory, ",");
		    }
				printf("%s\n", directory);
				closedir(d);
			}
			nbytes = sendto(sock, &directory, MAXBUFSIZE, 0, (struct sockaddr *) &remote, sizeof(remote));
		}
	}
	close(sock);
    return 0;
}
