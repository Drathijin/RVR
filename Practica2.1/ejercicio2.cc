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
	if(argn < 2)
	{
		fprintf(stderr,"Usage: ./%s [IP] [PORT]\n",argv[0]);
		return 1;
	}
	char* ip = argv[1];
	char* port = argv[2];

	printf("IP: %s - Port: %s\n", ip, port);
	addrinfo hints, *result;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags    = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family   = AF_INET; // IPv4
	hints.ai_socktype = SOCK_DGRAM;
	
	int rc =getaddrinfo(ip,port,&hints,&result);
	if (rc != 0){
		fprintf(stderr,"Something went wrong with gettaddrinfo: %s\n", gai_strerror(rc));		
		return 1;
	}
	int sd = socket(result->ai_family, result->ai_socktype, 0);
	if(sd < 0)
	{
		const char* name = "Init";
		fprintf(stderr, "Something went wrong with the initialization of the socket:\n\t");
		perror(name);
		return sd;
	}
	int b = bind(sd, result->ai_addr, result->ai_addrlen);
	if(b < 0)
	{
		const char* name = "Bind";
		fprintf(stderr, "Something went wrong with the binding of the socket:\n\t");
		perror(name);
		return b;
	}
	else printf("Bind succesfull!\n");
	freeaddrinfo(result);
	bool exit = false;
	int bytes;
	char buffer[80];
	sockaddr_storage client;
	// memset(&client, 0, sizeof(addrinfo));
	socklen_t client_len;
	char host[NI_MAXHOST], serv[NI_MAXSERV]; 
	while(!exit)
	{
		client_len = sizeof(client);
		bytes = recvfrom(sd, buffer,79,0,(struct sockaddr*)&client, &client_len);
		if(bytes == -1)
		{
			fprintf(stderr, "Something went wrong with the conection of the socket:\n\t");
			continue;
		}
		buffer[bytes] ='\0';
		
		int err = getnameinfo((struct sockaddr*)&client, client_len,host,NI_MAXHOST,serv,NI_MAXSERV,NI_NUMERICHOST);
		if (err != 0){
			fprintf(stderr,"Something went wrong with getnameinfo:\n\t%s\n",gai_strerror(err));
			return 1;
		}
		if (buffer[0] == 'q')
			exit = true;
		else 
		{
			tm *t;
			time_t t1 = time(0);
			t = localtime(&t1);
			char str[255];
			int strBytes;
			if(buffer[0] == 't')
			{
				strBytes = strftime(str,255,"%I:%M:%S %p",t);
			}
			else if(buffer[0] == 'd')
			{
				strBytes = strftime(str,255,"%Y-%m-%d",t);
			}
			else
				continue;
			sendto(sd, str,strBytes,0,(sockaddr*)&client, client_len);
			char clstr[1024];
			inet_ntop(client.ss_family, &client, clstr, client_len);
			printf("%d bytes de %s:%s\n", bytes, host,serv);
		}
	}

	return 0;
}