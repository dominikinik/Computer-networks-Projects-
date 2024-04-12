//Dominik Olejarz
//329405    

#ifndef INCOMING_PACKETS_H
#define INCOMING_PACKETS_H
#include <netinet/ip.h>
#include <chrono>
using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::milliseconds;
int calculateDuration(const std::chrono::time_point<Clock> &start, const std::chrono::time_point<Clock> &end);

int incoming_packet(int sockfd, int ttl, struct sockaddr_in *last_router, long total_time, std::chrono::time_point<std::chrono::high_resolution_clock> tab[3]);

#endif