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
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <queue>
#include <sys/ioctl.h>
#include <signal.h>

class EchoConnectionHandler
{
	static constexpr uint32_t MAX_BUFFER_SIZE = 1024;

public:
	EchoConnectionHandler(int sd) : _sd(sd), _thr(&EchoConnectionHandler::handleMessage, this){};
	virtual ~EchoConnectionHandler()
	{
		if (_thr.joinable()) //if you have not joined already
		{
			if (fcntl(_sd, F_GETFD) == O_NONBLOCK) //close your file descriptor it its open
				close(_sd);
			_thr.detach();
		}
	};

private:
	void handleMessage()
	{
		int bytes;
		bool exit = false;
		while (!exit)
		{
			bytes = recv(_sd, _buffer, MAX_BUFFER_SIZE - 1, 0);
			_buffer[bytes] = '\0'; //Make it a NULL terminated string just in case
			if (bytes == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
					continue;
				else
					handleError("Recv");
			}
			if (_buffer[0] == 'q' && bytes == 2)
			{
				exit = true;
			}
			else
				send(_sd, _buffer, bytes, 0);
		}
		printf("Desconexión de socket %d\n", _sd);
		close(_sd);
		_thr.detach();
	}

	int _sd;
	std::thread _thr;
	char _buffer[MAX_BUFFER_SIZE];
	sockaddr_storage _client;
};

std::queue<EchoConnectionHandler *> handlerList;
void handleSignal(int)
{
	printf("Conexión terminada\n");

	auto it = handlerList.front();
	while (!handlerList.empty())
	{
		delete handlerList.front();
		handlerList.pop();
	}
	exit(EXIT_SUCCESS);
}

int main(int argn, char *argv[])
{
	if (argn < 2)
	{
		printTCPServerUsage(argv[0]);
	}
	char *ip = argv[1];
	char *port = argv[2];
	int rc, sd;

	printf("IP: %s - Port: %s\n", ip, port);
	addrinfo hints, *result;

	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family = AF_INET;	 // IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if ((rc = getaddrinfo(ip, port, &hints, &result)) != 0)
		handleGAIError("getaddrinfo", rc);

	if ((sd = socket(result->ai_family, result->ai_socktype, IPPROTO_TCP)) < 0)
		handleError("Socket");

	if (bind(sd, result->ai_addr, result->ai_addrlen) < 0)
		handleError("Bind");
	else
		printf("Listening for conections!\n");

	if (listen(sd, 0) < 0)
		handleError("Listen");
	freeaddrinfo(result);

	int bytes, cliente_sd;
	char buffer[80];
	sockaddr_storage client;
	memset(&client, 0, sizeof(addrinfo));
	socklen_t client_len;
	char host[NI_MAXHOST], serv[NI_MAXSERV];
	client_len = sizeof(client);

	signal(SIGINT, handleSignal);
	while (true)
	{
		if ((cliente_sd = accept4(sd, (struct sockaddr *)&client, &client_len, O_NONBLOCK)) == -1)
			handleError("Accept");

		if ((rc = getnameinfo((struct sockaddr *)&client, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST)) != 0)
			handleGAIError("getnameinfo", rc);

		printf("Conexión desde %s %s\n", host, serv);

		handlerList.push(new EchoConnectionHandler(cliente_sd));
	}
}