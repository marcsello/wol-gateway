//
// Created by marcsello on 20/01/2021.
//

#ifndef WAKEONLANGATEWAY_WOL_H
#define WAKEONLANGATEWAY_WOL_H

#define SEND_WOL_SUCCESS 0
#define SEND_WOL_ERROR_INVALID_MAC 1
#define SEND_WOL_ERROR_SEND 2
#define SEND_WOL_ERROR_SENDTO_SOCKET 3
#define SEND_WOL_ERROR_SEND_SENDTO 4

#define MAC_STRING_LEN 17

int send_wol(const char *mac_str);

#endif //WAKEONLANGATEWAY_WOL_H
