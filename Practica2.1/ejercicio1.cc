#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "utils.h"

int main(int argn, char* argv[]){
	if(argn < 1)
	{
		printError("Usage: ./%s host",argv[0]);
		exit(EXIT_FAILURE);
	}

	char hbuf[NI_MAXHOST];
	
	addrinfo *info, *rp;
	int rc;

	if (rc=getaddrinfo(argv[1],NULL,NULL,&info) != 0)
		handleGAIError("getaddrinfo", rc);
	else{
		for(rp = info; rp !=NULL; rp=rp->ai_next){
			if((rc = getnameinfo(rp->ai_addr, rp->ai_addrlen,hbuf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST))!=0)
				handleGAIError("getnameinfo", rc);		
			printf("Host=%s, Family=%d, SocketType=%d\n", hbuf,rp->ai_family, rp->ai_socktype);
		}
	}
	return 0;
}
