// Dominik Olejarz
// 329405

#include "icmp_send.h"
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <poll.h>
#include <unistd.h>

using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::milliseconds;
bool unicalping(std::string incomingips[3], std::string ip,
                int numberofindividualpings) {
  for (int i = 0; i < numberofindividualpings; i++) {
    if (incomingips[i] == ip || incomingips[i] == "") {
      return false;
    }
  }
  return true;
}

int calculateDuration(const std::chrono::time_point<Clock> &end,
                      const std::chrono::time_point<Clock> &start) {
  auto diff = std::chrono::duration_cast<Duration>(end - start);
  return diff.count();
}
int incoming_packet(int sockfd, int ttl, struct sockaddr_in *last_router,
                    long total_time, std::chrono::time_point<Clock> tab[3]) {
  std::string incomingips[3];
  for (int i = 0; i < 3; i++)
    incomingips[i] = "";

  int isfinal = 1;
  auto timewaitstart = Clock::now();
  auto accttime = timewaitstart;
  char iptoprint[20];
  int recivedpings = 0;
  int numberofindividualpings = 0;
  while (calculateDuration(accttime, timewaitstart) <= 1000 and
         recivedpings < 3) {
    accttime = Clock::now();
    auto POLLTIMER = calculateDuration(accttime, timewaitstart);
    struct pollfd descriptors[1];
    descriptors[0].fd = sockfd;
    descriptors[0].events = POLLIN;

    if (poll(descriptors, 1, POLLTIMER)) {
      struct sockaddr_in router;
      socklen_t router_len = sizeof(router);
      u_int8_t buffer[IP_MAXPACKET];
      ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                                    (struct sockaddr *)&router, &router_len);
      if (packet_len < 0) {
        std::cout << "Error receiving packet\n";
        return EXIT_FAILURE;
      }
      if (inet_ntop(AF_INET, &(router.sin_addr), iptoprint,
                    sizeof(iptoprint)) == NULL) {
        std::cerr << "inet_ntop error: " << strerror(errno) << std::endl;
        return EXIT_FAILURE;
      }
      std::string strtowrite = std::string(iptoprint);

      struct iphdr *ip_hdr = (struct iphdr *)buffer;
      u_int8_t *pom_buffer = buffer + 4 * ip_hdr->ihl + 8;
      struct ip *pakiet = (struct ip *)pom_buffer;
      pom_buffer += (pakiet->ip_hl * 4);
      struct icmp *ICMP_REC = (struct icmp *)pom_buffer;
      u_int8_t *full_icmp = buffer + 4 * ip_hdr->ihl;
      struct icmphdr *icmp_hdr = (struct icmphdr *)full_icmp;

      if (icmp_hdr->type == ICMP_TIME_EXCEEDED) {
        int ttlodp = ICMP_REC->icmp_seq;
        int id = ICMP_REC->icmp_id;

        if (getpid() == id && ttl == ttlodp) {
          auto end_time = Clock::now();
          if (unicalping(incomingips, strtowrite, numberofindividualpings)) {
            incomingips[numberofindividualpings] = strtowrite;
            numberofindividualpings += 1;
          }

          total_time += calculateDuration(end_time, tab[recivedpings]);
          recivedpings++;
        }
      }

      else if (icmp_hdr->type == ICMP_ECHOREPLY) {
        int ttlodp = icmp_hdr->un.echo.sequence;
        int id = icmp_hdr->un.echo.id;

        if (getpid() == id && ttl == ttlodp) {
          auto end_time = Clock::now();
          if (unicalping(incomingips, strtowrite, numberofindividualpings)) {
            incomingips[numberofindividualpings] = strtowrite;
            numberofindividualpings += 1;
          }
          total_time += calculateDuration(end_time, tab[recivedpings]);
          recivedpings++;
          isfinal = 2;
        }
      }
    }
    accttime = Clock::now();
  }
  // std::cout << recivedpings << std::endl;
  if (recivedpings == 3) {
    for (int i = 0; i < numberofindividualpings; i++)
      std::cout << incomingips[i] << "  ";
    std::cout << total_time / 3 << "ms" << std::endl;
    return isfinal;
  } else if (recivedpings == 0) {
    std::cout << " *\n";
    return isfinal;
  } else {
    for (int i = 0; i < numberofindividualpings; i++)
      std::cout << incomingips[i] << "  ";
    std::cout << " ? ms\n";
    return isfinal;
  }
}