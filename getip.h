#ifndef GETIP_H
#define GETIP_H


#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include<arpa/inet.h>

struct hostent get_host(char*addr, struct hostent * h);

#endif
