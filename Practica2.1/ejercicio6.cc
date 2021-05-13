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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <threads.h>



int main(int argc, char const *argv[])
{
	/* code */
	return 0;
}
