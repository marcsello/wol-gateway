//
// Created by marcsello on 20/01/2021.
//
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include "wol.h"

#define MAC_STRING_LEN 17
#define BYTES_IN_MAC 6
#define WOL_HEADER_LEN 6
#define MAGIC_PACKET_MAC_REPETITIONS 16
#define MAGIC_PACKET_LEN WOL_HEADER_LEN + (BYTES_IN_MAC*MAGIC_PACKET_MAC_REPETITIONS)

#define UDP_SEND_SUCCESS 0
#define UDP_SEND_ERROR_SOCKET 1
#define UDP_SEND_ERROR_SENDTO 2

int udp_send(const unsigned char *msg, size_t msg_size, const char *target, int port) {

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        return UDP_SEND_ERROR_SOCKET;
    }

    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
    struct sockaddr_in servaddr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = inet_addr(target),
            .sin_port = htons(port),
    };

    int ret = UDP_SEND_ERROR_SENDTO;

    if (sendto(sock, msg, msg_size, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        ret = UDP_SEND_SUCCESS;
    }
    close(sock);
    return ret;
}


int parse_mac(const char *mac_str, char *mac_bin) {

    if (strnlen(mac_str, MAC_STRING_LEN+1) != MAC_STRING_LEN) { // +1 allows to detect string being too long
        return 0;
    }

    // tokenize includes null terminators to the string, that would violate the const thing above
    char mac_str_work[MAC_STRING_LEN + 1];
    strcpy(mac_str_work, mac_str);

    // Extract bytes from string
    char *token;
    token = strtok(mac_str_work, ":");
    int i = 0;

    while ((token != NULL) && (i < BYTES_IN_MAC)) {
        unsigned long a = strtoul(token, NULL, 16);

        // ghetto way to check if it's not larger than a byte
        // no need to check lower bound because unsigned variables
        if (a > 255) {
            return 0;
        }

        mac_bin[i] = (unsigned char) a; // NOLINT(cppcoreguidelines-narrowing-conversions)

        token = strtok(NULL, ":");
        i++;
    }

    return i == BYTES_IN_MAC;
}

int craft_magic_packet(const char *mac_str, unsigned char *outbuf) {
    // payload generation is based on
    // https://en.wikipedia.org/wiki/Wake-on-LAN#Magic_packet
    char mac_bin[BYTES_IN_MAC];
    if (!parse_mac(mac_str, mac_bin)) {
        return 0;
    }

    // Header
    memset(outbuf, 0xff, WOL_HEADER_LEN);

    // The mac address 16 times
    for (int i = 0; i < MAGIC_PACKET_MAC_REPETITIONS; i++) {
        memcpy(outbuf + (i * BYTES_IN_MAC) + WOL_HEADER_LEN, mac_bin, BYTES_IN_MAC);
    }

    return 1;
}

int send_wol(const char *mac_str) {
    unsigned char wol_payload[MAGIC_PACKET_LEN];

    if (!craft_magic_packet(mac_str, wol_payload)) {
        return SEND_WOL_ERROR_INVALID_MAC;
    }

    int sendret = udp_send(wol_payload, MAGIC_PACKET_LEN, "255.255.255.255", 9);

    switch (sendret) {
        case UDP_SEND_SUCCESS:
            return SEND_WOL_SUCCESS;

        case UDP_SEND_ERROR_SOCKET:
            return SEND_WOL_ERROR_SENDTO_SOCKET;

        case UDP_SEND_ERROR_SENDTO:
            return SEND_WOL_ERROR_SEND_SENDTO;

        default:
            return SEND_WOL_ERROR_SEND;
    }

}