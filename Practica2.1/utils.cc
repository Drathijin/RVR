#include "utils.h"
#include <stdarg.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
	\brief Prints as in printf but with white text over red BG
	@param ftm > same as in printf
	@param ... > same as in printf
*/
void printError(const char* fmt, ...)
{
	int fmtSize = strlen(fmt);
	char first[] = "\033[97;41m";
	char last[] = "\033[0m";
	fmtSize += strlen(first) + strlen(last);

	char newfmt[fmtSize];
	snprintf(newfmt, fmtSize,"%s%s%s",first, fmt, last);

	va_list args;
	va_start(args, fmt);
	vfprintf(stderr,newfmt, args);
	va_end(args);
}

/*
	\brief Prints error in errno with name of function
	@param name > Name of function where error ocurred
*/
void handleError(const char* name)
{
	printError("Something went wrong with %s: %s", name, strerror(errno));
	exit(EXIT_FAILURE);
}

/*
	\brief GAIPrints error in err with name of function
	@param name > Name of function where error ocurred
	@param err > GAI return value
*/
void handleGAIError(const char* name, int err)
{
	printError("Something went wrong with %s\t%s\n",name,gai_strerror(err));
	exit(EXIT_FAILURE);
}

void printUDPServerUsage(const char* name)
{
	fprintf(stderr,"Usage: ./%s [IP] [PORT]\n",name);
	exit(EXIT_FAILURE);
}
void printUDPClientUsage(const char* name)
{
	printf("Usage: ./%s [IP] [PORT] [Command] \n",name);
	printf("\t Commands:\n \t\t>[t] get time\n\t\t>[d] get day\n\t\t>[q] quit server\n");
	exit(EXIT_FAILURE);
}

void printTCPServerUsage(const char* name)
{
	printUDPServerUsage(name);
	exit(EXIT_FAILURE);
}

void printTCPClientUsage(const char* name)
{
	printUDPServerUsage(name);
	exit(EXIT_FAILURE);
}