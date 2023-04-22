#ifndef BETTING_GAME_COMMON_H
#define BETTING_GAME_COMMON_H

#include <stdint.h>

#define TRUE                  1
#define FALSE                 0
#define SERV_PORT             2222
#define INVALID_SOCKET        -1
#define SOCKET_ERROR          -1

#define BETSERVER_NUM_CLIENTS 40
#define BETSERVER_NUM_MIN     0xe0ffff00
#define BETSERVER_NUM_MAX     0xe0ffffaa
#define PROTOCOL_VERSION      0x1

/* Message Types */
enum {
    BETTING_GAME_MSG_NONE,
    BETTING_GAME_MSG_OPEN,
    BETTING_GAME_MSG_ACCEPT,
    BETTING_GAME_MSG_BET,
    BETTING_GAME_MSG_RESULT,
};

#undef BETTING_SERVER_TRACE_ON
#undef BETTING_CLIENT_TRACE_ON

#define BETTING_SERVER_TRACE_ON
#define BETTING_CLIENT_TRACE_ON

#ifdef BETTING_SERVER_TRACE_ON
#define SERVER_TRACE_LOG(fmt, args...) \
    do                           \
    {                            \
        printf(fmt, ##args);      \
    } while (0)
#else
#define SERVER_TRACE_LOG(fmt, args...) ;
#endif

#ifdef BETTING_CLIENT_TRACE_ON
#define CLIENT_TRACE_LOG(fmt, args...) \
    do                           \
    {                            \
        printf(fmt, ##args);      \
    } while (0)
#else
#define CLIENT_TRACE_LOG(fmt, args...) ;
#endif


/**
 * 0                   1                   2                   3
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Lower end of number range                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                   Upper end of number range                   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct betting_game_msg_accept
{
    unsigned long lower_end_of_number;
    unsigned long upper_end_of_num;
} betting_game_msg_accept_t;


/**
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Betting number                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct betting_game_msg_bet
{
    unsigned long client_bet;
} betting_game_msg_bet_t;


/**
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ver |  Len    |    Type       |        Client Id              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
typedef struct betting_game_common_hdr
{
    unsigned version : 3;
    unsigned length : 5;
    unsigned char type;
    unsigned short client_id;
} betting_game_common_hdr_t;

/**
 *   0                   1                   2                   3
 *   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |    Status     |                Winning number...              |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  | Win num cont. |
 *  +-+-+-+-+-+-+-+-+
 */
typedef struct betting_game_result
{
    unsigned char result_status;
    unsigned long winning_number;
} betting_game_result_t;


typedef struct betting_game_client
{
    int sockfd;           /*!< socket file decriptor */
    int state;            /*!< state of client aka last received message */
    long int betting_num; /*!< betting number */
} betting_game_client_t;


void
make_header(
    unsigned protocol_ver,
    unsigned pkt_type,
    uint8_t pkt_size,
    uint16_t client_id,
    betting_game_common_hdr_t *send_hdr);

int
gen_betting_number(
    int min,
    int max);

void
error(
    char *msg);


#endif

