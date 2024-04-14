// Dominik Olejarz
// 329405

#include "icmp_send.h"
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/socket.h>
#include <unistd.h>

u_int16_t compute_icmp_checksum(const void *buff, int length) {
  const u_int16_t *ptr = (const u_int16_t *)buff;
  u_int32_t sum = 0;
  assert(length % 2 == 0);
  for (; length > 0; length -= 2) {
    sum += *ptr++;
  }
  sum = (sum >> 16U) + (sum & 0xffffU);
  return (u_int16_t)(~(sum + (sum >> 16U)));
}

bool send_packets(int sockfd, int ttl, char *dest) {
  pid_t pid;

  pid = getpid();
  if (pid < 0) {
    if (errno == EPERM) {
      std::cerr << "Insufficient privileges to obtain the PID." << std::endl;
    } else {
      std::cerr << "An error occurred while obtaining the PID." << std::endl;
    }
    return false;
  }
  struct icmp header;
  header.icmp_type = ICMP_ECHO;
  header.icmp_code = 0;
  header.icmp_hun.ih_idseq.icd_id = pid;
  header.icmp_hun.ih_idseq.icd_seq = ttl;
  header.icmp_cksum = 0;
  header.icmp_cksum =
      compute_icmp_checksum((u_int16_t *)&header, sizeof(header));

  struct sockaddr_in recipient;
  bzero(&recipient, sizeof(recipient));
  recipient.sin_family = AF_INET;
  if (inet_pton(AF_INET, dest, &recipient.sin_addr) <= 0) {
    if (inet_pton(AF_INET, dest, &recipient.sin_addr) == 0) {
      std::cout << "Invalid IP address." << std::endl;

    } else {
      perror("Error converting IP address");
    }
    return false;
  }
  if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0) {
    perror("Error setting TTL value");
    return false;
  }

  if (sendto(sockfd, &header, sizeof(header), 0, (struct sockaddr *)&recipient,
             sizeof(recipient)) < 0) {
    std::cerr << "sendto error: " << strerror(errno) << std::endl;
    return false;
  }
  return true;
}