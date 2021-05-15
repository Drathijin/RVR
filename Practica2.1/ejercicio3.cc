/*
Cliente tiempo UDP
Características:
	Manda un comando al servidor UDP (ejercicio 4) que devuelve el tiempo actual (t) o el día (d)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "utils.h"
int main(int argn, char* argv[]){
	if(argn < 4)
	{
		printUDPClientUsage(argv[0]);
	}

	char* ip = argv[1];
	char* port = argv[2];
	char* command = argv[3];
	printf("Trying to connect to IP: %s - Port: %s\n", ip, port);

	int sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sd == -1)
		handleError("socket");


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
			handleError("recvfrom");
		}
		buffer[bytes] ='\0';
		printf("%s\n",buffer);
	}
	return 0;
}