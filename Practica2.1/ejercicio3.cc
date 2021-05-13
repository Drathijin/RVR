 #include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

int main(int argn, char* argv[]){
	if(argn < 4)
	{
		printf("Usage: ./%s [IP] [PORT] [Command] \n",argv[0]);
		printf("\t Commands:\n \t\t>[t] get time\n\t\t>[d] get day\n\t\t>[q] quit server\n");
		return 1;
	}

	char* ip = argv[1];
	char* port = argv[2];
	char* command = argv[3];
	printf("Trying to connect to IP: %s - Port: %s\n", ip, port);

	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sd == -1)
	{
		const char* name = "Init";
		fprintf(stderr, "Something went wrong with the initialization of the socket:\n\t");
		perror(name);
		return sd;
	}


	sockaddr_in servaddr;
	socklen_t servlen = sizeof(servaddr);
	memset(&servaddr, 0, sizeof(sockaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	inet_aton(ip, &servaddr.sin_addr );

	char buffer[80];
	sendto(sd, command,strlen(command),0,(sockaddr*)&servaddr, servlen);
	if (command[0] != 'q')
	{
		int bytes = recvfrom(sd, buffer,79,0,(struct sockaddr*)&servaddr, &servlen);
		if(bytes == -1)
		{
			fprintf(stderr, "Something went wrong with the conection to server:\n\t");
			return bytes;
		}
		buffer[bytes] ='\0';
		printf("%s\n",buffer);
	}
	return 0;
}