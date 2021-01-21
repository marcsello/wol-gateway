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


int udp_send(const unsigned char *msg, size_t msg_size, const char *target, int port) {

    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    if (sock < 0) {
        return 0;
    }

    int yes = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes));
    struct sockaddr_in servaddr = {
            .sin_family = AF_INET,
            .sin_addr.s_addr = inet_addr(target),
            .sin_port = htons(port),
    };

    int ret = 1;

    if (sendto(sock, msg, msg_size, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        ret = 0;
    }
    close(sock);
    return ret;
}


int parse_mac(const char *mac_str, char *mac_bin) {

    if (strnlen(mac_str, 20) != 17) {
        return 0;
    }

    char mac_str_work[17 + 1];
    strcpy(mac_str_work, mac_str);

    char *token;
    token = strtok(mac_str_work, ":");
    int i = 0;

    while ((token != NULL) && (i < 6)) {
        unsigned long a = strtoul(token, NULL, 16);

        if (a > 255) {
            return 0;
        }

        mac_bin[i] = (unsigned char) a;

        token = strtok(NULL, ":");
        i++;
    }

    return i == 6;
}

void craft_magic_packet(const char *mac_str, unsigned char *outbuf) {
    char mac_bin[6];
    parse_mac(mac_str, mac_bin); // TODO: check return value

    memset(outbuf, 0xff, 6);

    for (int i = 0; i < 16; i++) {
        memcpy(outbuf + (i * 6) + 6, mac_bin, 6);
    }

}

void send_wol(const char *mac_str) {
    unsigned char wol_payload[102];
    craft_magic_packet(mac_str, wol_payload);

    printf("Sending WoL to %s...\n",mac_str);
    if (!udp_send(wol_payload, 102, "255.255.255.255", 9)) {
        printf("Failed to send WoL packet...\n");
    }
}