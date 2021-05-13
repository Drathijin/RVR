#ifndef _UTILS_H__
#define _UTILS_H__

void printUDPClientUsage(const char*);
void printUDPServerUsage(const char*);

void printTCPClientUsage(const char*);
void printTCPServerUsage(const char*);

void handleError(const char*);
void handleGAIError(const char*,int);

void printError(const char* fmt, ...);

#endif 