/*
Servidor UPD (ejercicio 2) multi-thread
Características:
	Escucha comandos [t] [d] y responde con tiempo, día o cierra el servidor.
	Sigue un modelo pre-fork
	Clase propia, constructor únicamente sd (socket descriptor), un único método de procesado de mensajes
	Va a tener un sleep que simule carga de proceso de mensajes e imprimir el std::this_tread::get_id()
	El thread principal lee hasta que reciba 'q' entonces cierra todos los threads y el servidor
*/

#include <time.h>
#include <netdb.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <thread>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>
#include "utils.h"

const unsigned int NUM_THREADS = 5;
bool STATUS = true;

class messageHandler
{
public:
	messageHandler() {}
	messageHandler(int sd) : _sd(sd), t(&messageHandler::handleMessage, this) {IDS[this] = NextID; NextID++;}
	~messageHandler() { t.join(); }
	void handleMessage()
	{
		int bytes, rc;
		char buffer[80];
		sockaddr_storage client;
		socklen_t client_len;
		char host[NI_MAXHOST], serv[NI_MAXSERV];
		while (STATUS)
		{
			client_len = sizeof(client);
			if ((bytes = recvfrom(_sd, buffer, 79, 0, (struct sockaddr *)&client, &client_len)) == -1)
			{
				if (errno != EAGAIN && errno != EWOULDBLOCK)
					handleError("recvfrom [async] ");
				else
					continue; //All good, just loop again to check for STATUS
			}
			else //We recieved something
				buffer[1] = '\0';

			if ((rc = getnameinfo((struct sockaddr *)&client, client_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST)) != 0)
				handleGAIError("getnameinfo", rc);
			if (buffer[0] == 't' || buffer[0] == 'd')
			{
				tm *t;
				time_t t1 = time(0);
				t = localtime(&t1);
				char str[255];
				int strBytes;
				if (buffer[0] == 't')
					strBytes = strftime(str, 255, "%I:%M:%S %p", t);
				else if (buffer[0] == 'd')
					strBytes = strftime(str, 255, "%Y-%m-%d", t);
				sendto(_sd, str, strBytes, 0, (sockaddr *)&client, client_len);
				printf("%d bytes de %s:%s\n \tHandled by: %d\n", bytes, host, serv, IDS[this]);
				sleep(3);
			}
		}
	}
private:

	static std::map<messageHandler*, int>IDS;
	static int NextID;
	int _sd;
	std::thread t;
};

//Init static members
int messageHandler::NextID = 0;
std::map<messageHandler*, int> messageHandler::IDS = std::map<messageHandler*, int>();

int main(int argc, char const *argv[])
{
	if (argc < 2)
		printUDPServerUsage(argv[0]);
	const char *ip = argv[1];
	const char *port = argv[2];

	addrinfo hints, *result;
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags = AI_PASSIVE; //Devolver 0.0.0.0
	hints.ai_family = AF_INET;	 // IPv4
	hints.ai_socktype = SOCK_DGRAM;

	int rc;
	if (rc = getaddrinfo(ip, port, &hints, &result) != 0)
		handleGAIError("getaddrinfo", rc);

	int sd;
	if ((sd = socket(result->ai_family, result->ai_socktype | SOCK_NONBLOCK, 0)) < 0)
		handleError("socket");

	int b;
	if ((b = bind(sd, result->ai_addr, result->ai_addrlen)) < 0)
		handleError("bind");

	freeaddrinfo(result);

	messageHandler *handlers[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++)
		handlers[i] = new messageHandler(sd);

	char c;
	while (std::cin >> c)
		if (c == 'q')
			break;
	STATUS = false;
	for (int i = 0; i < NUM_THREADS; i++)
		delete handlers[i];

	close(sd);
	return 0;
}
