/*
Servidor tiemo UDP
Características:
	Escucha comandos [t] [d] y responde con tiempo, día o cierra el servidor.
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
	if(argn < 2)
	{
		printUDPServerUsage(argv[0]);
	}
	char* ip = argv[1];
	char* port = argv[2];

	addrinfo hints, *result;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family   = AF_INET; // IPv4
	hints.ai_socktype = SOCK_DGRAM;
	int rc;
	if (rc = getaddrinfo(ip,port,&hints,&result) != 0)
		handleGAIError("getaddrinfo", rc);
	
	int sd;
	if((sd = socket(result->ai_family, result->ai_socktype, 0)) < 0)
		handleError("socket");
	
	int b;
	if((b=bind(sd, result->ai_addr, result->ai_addrlen)) < 0)
		handleError("bind");

	freeaddrinfo(result);

	bool exit = false;
	int bytes;
	char buffer[80];
	sockaddr_storage client;
	socklen_t client_len;
	char host[NI_MAXHOST], serv[NI_MAXSERV]; 
	while(!exit)
	{
		client_len = sizeof(client);
		if((bytes = recvfrom(sd, buffer,79,0,(struct sockaddr*)&client, &client_len)) == -1)
			handleError("recvfrom");

		buffer[1] ='\0';
		
		if ((rc=getnameinfo((struct sockaddr*)&client, client_len,host,NI_MAXHOST,serv,NI_MAXSERV,NI_NUMERICHOST)) != 0)
			handleGAIError("getnameinfo",rc);
		if (buffer[0] != 'q')
		{
			tm *t;
			time_t t1 = time(0);
			t = localtime(&t1);
			char str[255];
			int strBytes;
			if(buffer[0] == 't')
				strBytes = strftime(str,255,"%I:%M:%S %p",t);
			else if(buffer[0] == 'd')
				strBytes = strftime(str,255,"%Y-%m-%d",t);
			else
				continue;

			sendto(sd, str,strBytes,0,(sockaddr*)&client, client_len);;
			printf("%d bytes de %s:%s\n", bytes, host,serv);
		}
		else 
			exit = true;
	}

	return 0;
}