//Dominik Olejarz
//329405    

#include "icmp_send.h"
#include "incoming_packets.h"
#include <cstdio>
#include <cstdlib>
#include <netinet/ip.h>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#define ICMP_ECHO 8
#define IP_MAXPACKET 65535
#define MAX_TTL 30
#define NUM_PACKETS 3

using Clock = std::chrono::high_resolution_clock;
int main(int argc, char *argv[])
{
    std::chrono::time_point<Clock> tab[3];
    long total_time = 0;
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <destination>\n";
        return EXIT_FAILURE;
    }

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
        std::cerr << "socket error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in last_router;

    for (int ttl = 1; ttl <= MAX_TTL; ttl++)
    {
        total_time = 0;
        

        for (int i = 0; i < NUM_PACKETS; i++)
        {
            if(!send_packets(sockfd, ttl, argv[1])){return EXIT_FAILURE;} 
            tab[i] = Clock::now();
        }
        
        if (incoming_packet(sockfd, ttl, &last_router, total_time, tab) == 2)
        {
            close(sockfd);
            return EXIT_SUCCESS;
        }
    }
}