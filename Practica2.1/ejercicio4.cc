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
		printTCPServerUsage(argv[0]);
	}
	char* ip = argv[1];
	char* port = argv[2];
	int rc, sd;

	printf("IP: %s - Port: %s\n", ip, port);
	addrinfo hints, *result;
	
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family   = AF_INET; // IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	if ((rc=getaddrinfo(ip,port,&hints,&result)) != 0){
		handleGAIError("getaddrinfo",rc);
	}

	if((sd = socket(result->ai_family, result->ai_socktype, IPPROTO_TCP)) < 0)
		handleError("Socket");

	if(bind(sd, result->ai_addr, result->ai_addrlen) < 0)
		handleError("Bind");
	else printf("Listening for conections!\n");

	if(listen(sd,0) < 0)
		handleError("Listen");
	freeaddrinfo(result);

	bool exit = false;
	int bytes, cliente_sd;
	char buffer[80];
	sockaddr_storage client;
	memset(&client, 0, sizeof(addrinfo));
	socklen_t client_len;
	char host[NI_MAXHOST], serv[NI_MAXSERV]; 
	client_len = sizeof(client);
	if((cliente_sd = accept(sd, (struct sockaddr*)&client, &client_len)) == -1)
		handleError("Accept");
	
	if((rc = getnameinfo((struct sockaddr*)&client, client_len,host,NI_MAXHOST,serv,NI_MAXSERV,NI_NUMERICHOST)) != 0){
		handleGAIError("getnameinfo",rc);
	}
	printf("Conexión desde %s %s\n", host, serv);

	while(!exit)
	{
		bytes=recv(cliente_sd, buffer, 79,0);
		if(bytes==-1)
			handleError("Recv");
		if(bytes == 0)
			exit = true;
		else
			sendto(cliente_sd, buffer,bytes,0,(sockaddr*)&client, client_len);
	}
		printf("Conexión terminada\n");

	return EXIT_SUCCESS;
}