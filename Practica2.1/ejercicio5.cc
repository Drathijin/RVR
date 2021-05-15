/*
Cliente echo TCP
Características:
	Manda una string del servidor e imprime su respuesta (un echo de la misma)
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include "utils.h"

int SOCKD;


void close_conection_and_exit(int sock)
{
	if (close(sock) == -1)
		handleError("close");
	exit(0);
}

void handle_quit(int sig)
{
	close_conection_and_exit(SOCKD);
}

int main(int argn, char *argv[])
{
	if (argn < 3)
	{
		printTCPClientUsage(argv[0]);
	}

	char *ip = argv[1];
	char *port = argv[2];
	char *command = argv[3];
	printf("Trying to connect to IP: %s - Port: %s\n", ip, port);

	int SOCKD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKD == -1)
		handleError("Socket");
	signal(SIGINT, handle_quit);

	sockaddr_in servaddr;
	socklen_t servlen = sizeof(servaddr);
	memset(&servaddr, 0, sizeof(sockaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(port));
	inet_aton(ip, &servaddr.sin_addr);
	char buffer[80];

	if (connect(SOCKD, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
		handleError("Connect");
	while (true)
	{
		int i = 0;
		char ch =' ';
		while(read(STDIN_FILENO, &ch, 1 > 0 && i < 79 && ch !=EOF && ch !='\n'))
		{
			buffer[i] = ch;
			i++;
		}
		buffer[i] = '\0'; 
		if (buffer[0] != 'q' && i > 1)
		{
			if (sendto(SOCKD, buffer, strlen(buffer), 0, (sockaddr *)&servaddr, servlen) == -1)
				handleError("SendTo");
			if (recv(SOCKD, buffer, i, 0) == -1)
				handleError("Recv");
			buffer[i] = '\0';
			printf("%s", buffer);
		}
		else
		{
			close_conection_and_exit(SOCKD);
		}
	}
	close_conection_and_exit(SOCKD);
}