#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argn, char* argv[]){
	if(argn < 1)
	{
		fprintf(stderr,"Usage: ./%s host",argv[0]);
		return 1;
	}

	socklen_t addrlen;
	char hbuf[NI_MAXHOST];
	
	addrinfo *info, *rp;
	int rc =getaddrinfo(argv[1],NULL,NULL,&info);
	if (rc != 0){
		fprintf(stderr,"Something went wrong with gettaddrinfo: %s", gai_strerror(rc));		
		return 1;
	}
	else{
		for(rp = info; rp !=NULL; rp=rp->ai_next){
		rc = getnameinfo(rp->ai_addr, rp->ai_addrlen,hbuf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
		if(rc == 0)
			printf("Host=%s, Family=%d, SocketType=%d\n", hbuf,rp->ai_family, rp->ai_socktype);
		else if (rc != 0){
			fprintf(stderr,"Something went wrong with getnameinfo: %s", gai_strerror(rc));		
			return 1;
			}
		}
	}
	return 0;
}
