//Dominik Olejarz
//329405    

#ifndef ICMP_SEND_H
#define ICMP_SEND_H
#include <netinet/ip.h>

u_int16_t compute_icmp_checksum(const void *buff, int length);
bool send_packets(int sockfd, int ttl, char *dest);
#endif